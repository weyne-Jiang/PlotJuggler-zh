# SPDX-License-Identifier: MPL-2.0
"""Reject incomplete or structurally unsafe Simplified Chinese Qt translations."""

from collections import Counter
from html.parser import HTMLParser
from pathlib import Path
import argparse
import re
import sys
import tempfile
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
PLACEHOLDER = re.compile(r'%(?:L?\d+|L?n)')
CJK = re.compile(r'[\u3400-\u9fff]')
# These are deliberately invariant: brand names, named colour maps, shortcuts,
# punctuation-only formatting templates, and a Designer stylesheet property.
INVARIANT = {
    # Reviewed brands, serialization names, numerical UI fragments and literals.
    'PlotJuggler', 'Lua', 'Python', 'CSV', 'Parquet', '0 of 0', ' of ',
    '+', '-', '?', 'X', 'Y', '...', '1', '2', '3', '4', '5', '6', '7', '8', '(*.qss)', '(*.proto)', '(*.*)',
}
HTML_TAGS = {'a', 'b', 'br', 'code', 'em', 'i', 'li', 'p', 'span', 'strong', 'ul', 'html', 'head', 'body', 'meta', 'style', 'font', 'table', 'tr', 'td'}
VOID_TAGS = {'br', 'meta'}


class Markup(HTMLParser):
    """Check Qt rich-text structure without mistaking XML diagnostic names for HTML."""

    def __init__(self):
        super().__init__(convert_charrefs=False)
        self.tags = Counter()
        self.stack = []
        self.errors = []
        self.links = []
        self.attributes = Counter()

    def handle_starttag(self, tag, attrs):
        if tag not in HTML_TAGS:
            return
        self.tags[tag] += 1
        self.attributes[(tag, tuple(sorted(attrs)))] += 1
        if tag not in VOID_TAGS:
            self.stack.append(tag)
        if tag == 'a':
            self.links.append(dict(attrs).get('href', ''))

    def handle_startendtag(self, tag, attrs):
        self.handle_starttag(tag, attrs)
        if tag not in VOID_TAGS and tag in HTML_TAGS:
            self.handle_endtag(tag)

    def handle_endtag(self, tag):
        if tag not in HTML_TAGS or tag in VOID_TAGS:
            return
        if not self.stack or self.stack.pop() != tag:
            self.errors.append('unbalanced rich-text tags')


def markup(text):
    """Return parsed markup, including unclosed tags."""
    parser = Markup()
    parser.feed(text)
    if parser.stack:
        parser.errors.append('unclosed rich-text tag')
    return parser


def mnemonics(text):
    """Count single ampersands; escaped literal && and HTML entities are not shortcuts."""
    plain = re.sub(r'&(?:#\d+|#x[0-9a-fA-F]+|[A-Za-z]+);', '', text)
    plain = plain.replace('&&', '')
    return len(re.findall(r'&[^\s&<]', plain))


def validate(tree):
    """Validate effective TS entries; obsolete entries are never shipped by lrelease."""
    errors = []
    root = tree.getroot()
    if root.get('language') != 'zh_CN':
        errors.append('TS language must be zh_CN')
    effective = 0
    keys = set()
    for context in root.findall('context'):
        name = context.findtext('name', '')
        for message in context.findall('message'):
            source = message.findtext('source', '')
            translation = message.find('translation')
            label = f'{name}: {source[:90]!r}'
            if translation is not None and translation.get('type') in {'obsolete', 'vanished'}:
                errors.append(f'{label}: obsolete/vanished translation')
                continue
            key = (name, source, message.findtext('comment', ''), message.get('numerus', 'no'))
            if key in keys:
                errors.append(f'{label}: duplicate message key')
            keys.add(key)
            effective += 1
            if translation is None or translation.get('type') == 'unfinished':
                errors.append(f'{label}: missing/unfinished translation')
                continue
            forms = translation.findall('numerusform')
            if message.get('numerus') == 'yes':
                if len(forms) != 1:
                    errors.append(f'{label}: Chinese numerus requires exactly one form')
                texts = [form.text or '' for form in forms]
            else:
                if forms:
                    errors.append(f'{label}: unexpected numerus form')
                texts = [''.join(translation.itertext())]
            for text in texts:
                if not text.strip():
                    errors.append(f'{label}: empty translation')
                if Counter(PLACEHOLDER.findall(source)) != Counter(PLACEHOLDER.findall(text)):
                    errors.append(f'{label}: changed placeholders')
                if source not in INVARIANT and not CJK.search(text):
                    errors.append(f'{label}: no Chinese text (add a reviewed invariant if appropriate)')
                if source == text and source not in INVARIANT:
                    errors.append(f'{label}: untranslated English')
                if source.count('\n') != text.count('\n'):
                    errors.append(f'{label}: changed line breaks')
                original, translated = markup(source), markup(text)
                if translated.errors or original.tags != translated.tags:
                    errors.append(f'{label}: changed/broken rich-text markup')
                if original.links != translated.links:
                    errors.append(f'{label}: changed rich-text link targets')
                if original.attributes != translated.attributes:
                    errors.append(f'{label}: changed rich-text attributes')
                if re.findall(r'<code>(.*?)</code>', source, re.S) != re.findall(r'<code>(.*?)</code>', text, re.S):
                    errors.append(f'{label}: modified code example')
                if re.findall(r'https?://[^\s<>\"\']+', source) != re.findall(r'https?://[^\s<>\"\']+', text):
                    errors.append(f'{label}: changed URL')
                if source.count('&&') != text.count('&&'):
                    errors.append(f'{label}: changed escaped ampersands')
                if mnemonics(source) != mnemonics(text):
                    errors.append(f'{label}: changed accelerator count')
    if not effective:
        errors.append('catalogue has no effective messages')
    return errors, effective


def catalogue_keys(tree):
    """Qt lookup identity includes context, source, disambiguation and plural flag."""
    return {(c.findtext('name', ''), m.findtext('source', ''),
             m.findtext('comment', ''), m.get('numerus', 'no'))
            for c in tree.findall('context') for m in c.findall('message')
            if m.find('translation') is None or
            m.find('translation').get('type') not in {'obsolete', 'vanished'}}


def validate_source_keys(tree, extracted):
    """Reject missing entries and stale contexts by comparison with native extraction."""
    actual, expected = catalogue_keys(tree), catalogue_keys(extracted)
    return ([f'missing source key: {key!r}' for key in sorted(expected - actual)] +
            [f'stale source key: {key!r}' for key in sorted(actual - expected)])


def main():
    """Exit nonzero if catalogue completeness or protected formatting regresses."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('ts', nargs='?', type=Path,
                        default=ROOT / 'plotjuggler_app/translations/plotjuggler_zh_CN.ts')
    parser.add_argument('--lupdate', help='native Qt lupdate; also verify exact source keys')
    args = parser.parse_args()
    try:
        tree = ET.parse(args.ts)
        errors, count = validate(tree)
        if args.lupdate:
            from update_translations import refresh_catalogue
            with tempfile.TemporaryDirectory(prefix='pj-validate-') as directory:
                extracted = Path(directory) / 'source_zh_CN.ts'
                refresh_catalogue(args.lupdate, extracted)
                errors.extend(validate_source_keys(tree, ET.parse(extracted)))
    except (ET.ParseError, OSError) as error:
        print(error, file=sys.stderr)
        return 1
    for error in errors:
        print(error, file=sys.stderr)
    if errors:
        print(f'FAILED: {len(errors)} errors in {count} messages.', file=sys.stderr)
        return 1
    print(f'Validated {count} complete zh_CN translations.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
