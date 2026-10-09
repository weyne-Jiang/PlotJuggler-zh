# SPDX-License-Identifier: MPL-2.0
"""Mutation tests prove the translation validator rejects broken TS fixtures."""

import unittest
import xml.etree.ElementTree as ET
import sys

sys.dont_write_bytecode = True
from validate_translations import validate, validate_source_keys


def fixture(source='Value %1', translated='数值 %1', numerus=False):
    """Build a tiny valid catalogue to mutate without touching the production TS."""
    root = ET.Element('TS', language='zh_CN')
    context = ET.SubElement(root, 'context')
    ET.SubElement(context, 'name').text = 'Fixture'
    message = ET.SubElement(context, 'message')
    ET.SubElement(message, 'source').text = source
    tr = ET.SubElement(message, 'translation')
    if numerus:
        message.set('numerus', 'yes')
        ET.SubElement(tr, 'numerusform').text = translated
    else:
        tr.text = translated
    return ET.ElementTree(root)


class TranslationValidationTests(unittest.TestCase):
    """Exercise failures that otherwise become subtle runtime translation defects."""

    def assert_rejected(self, tree, reason):
        errors, _ = validate(tree)
        self.assertTrue(any(reason in error for error in errors), errors)

    def test_valid_translation(self):
        self.assertEqual(validate(fixture()), ([], 1))

    def test_placeholder_loss_and_duplication(self):
        for text in ('数值', '数值 %1 %1', '数值 %2'):
            self.assert_rejected(fixture(translated=text), 'placeholders')

    def test_unfinished_and_empty(self):
        tree = fixture()
        tree.find('.//translation').set('type', 'unfinished')
        self.assert_rejected(tree, 'unfinished')
        self.assert_rejected(fixture(translated=''), 'empty')

    def test_fake_finished_english(self):
        self.assert_rejected(fixture(translated='Value %1'), 'untranslated English')

    def test_chinese_numerus(self):
        tree = fixture('%n curves', '%n 条曲线', numerus=True)
        self.assertEqual(validate(tree), ([], 1))
        ET.SubElement(tree.find('.//translation'), 'numerusform').text = '%n 条曲线'
        self.assert_rejected(tree, 'exactly one')

    def test_html_structure_link_and_code(self):
        self.assert_rejected(fixture('<b>Value %1</b>', '<b>数值 %1'), 'markup')
        self.assert_rejected(fixture('<a href="https://a">Value %1</a>',
                                     '<a href="https://b">数值 %1</a>'), 'link targets')
        self.assert_rejected(fixture('<span style="font-weight:600">Value %1</span>',
                                     '<span style="font-weight:400">数值 %1</span>'), 'attributes')
        self.assert_rejected(fixture('<code>sudo apt update</code> %1',
                                     '<code>sudo apt upgrade</code> 更新 %1'), 'code example')

    def test_accelerator_and_multiline(self):
        self.assert_rejected(fixture('&Open %1', '打开 %1'), 'accelerator')
        self.assert_rejected(fixture('Value\n%1', '数值 %1'), 'line breaks')

    def test_repeated_zero_localized_placeholders(self):
        tree = fixture('Code %0: %L1 / %1 / %1', '代码 %0：%L1 / %1 / %1')
        self.assertEqual(validate(tree), ([], 1))
        self.assert_rejected(fixture('Code %0 %1 %1', '代码 %0 %1'), 'placeholders')

    def test_escaped_ampersands_and_urls(self):
        self.assert_rejected(fixture('A && B', '甲与乙'), 'escaped ampersands')
        self.assert_rejected(fixture('Visit https://a.example/help',
                                     '访问 https://b.example/help'), 'URL')

    def test_obsolete_duplicates_and_source_identity(self):
        tree = fixture()
        tree.find('.//translation').set('type', 'obsolete')
        self.assert_rejected(tree, 'obsolete')
        tree = fixture()
        import copy
        tree.find('.//context').append(copy.deepcopy(tree.find('.//message')))
        self.assert_rejected(tree, 'duplicate')
        expected = fixture()
        actual = fixture()
        actual.find('.//name').text = 'WrongContext'
        errors = validate_source_keys(actual, expected)
        self.assertTrue(any('missing source key' in error for error in errors))
        self.assertTrue(any('stale source key' in error for error in errors))
        actual = fixture()
        ET.SubElement(actual.find('.//message'), 'comment').text = 'wrong disambiguation'
        self.assertTrue(validate_source_keys(actual, expected))

    def test_no_english_whitelist_bypass(self):
        self.assert_rejected(fixture('Unreviewed label', 'Unreviewed label'), 'untranslated')
        self.assert_rejected(fixture('Unreviewed label', 'different English'), 'no Chinese')


if __name__ == '__main__':
    unittest.main()
