# SPDX-License-Identifier: MPL-2.0
"""Refresh first-party Qt catalogues with lupdate's real C++/Designer parser."""

from pathlib import Path
import argparse
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
MODULES = ('plotjuggler_app', 'plotjuggler_base', 'plotjuggler_plugins')
EXCLUDED = {'tests', 'test', 'demos', 'demo', '3rdparty', 'translations', 'build', 'rosx_introspection'}
SUFFIXES = {'.cpp', '.cc', '.cxx', '.h', '.hpp', '.ui'}


def source_files():
    """Yield production sources only; never extract demo/test or upstream Qt text."""
    return sorted({
        path for module in MODULES for path in (ROOT / module).rglob('*')
        if path.is_file() and path.suffix in SUFFIXES
        and not EXCLUDED.intersection(path.relative_to(ROOT).parts)
        and not any(part.startswith('build') for part in path.relative_to(ROOT).parts)
    })


def include_roots():
    """Stable include roots shared with CMake's first-party extraction target."""
    return [ROOT / module / directory for module in MODULES
            for directory in ('', 'src', 'include')
            if (ROOT / module / directory).is_dir()]


def refresh_catalogue(lupdate, ts):
    """Extract an existing or fresh catalogue using the native Qt parser."""
    files = source_files()
    includes = include_roots()
    # Explicit include roots preserve the compiler's actual QObject identities.
    # PlotJuggler 3's main window is MainWindow (not PJ::MainWindow).
    lines = [str(path).replace('\\', '/') for path in files]
    lines += ['-I' + str(path).replace('\\', '/') for path in includes]
    with tempfile.TemporaryDirectory(prefix='pj-lupdate-') as directory:
        listing = Path(directory) / 'sources.lst'
        listing.write_text('\n'.join(lines) + '\n', encoding='utf-8')
        subprocess.run([lupdate, '@' + str(listing), '-no-obsolete',
                        '-locations', 'relative', '-source-language', 'en_US', '-target-language', 'zh_CN',
                        '-ts', str(ts)], check=True, cwd=ROOT)
    return len(files)


def main():
    """Run the selected Qt version without requiring a configured application build."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--lupdate', default=shutil.which('lupdate'),
                        help='lupdate executable from the same Qt version used to build')
    parser.add_argument('--ts', type=Path,
                        default=ROOT / 'plotjuggler_app/translations/plotjuggler_zh_CN.ts')
    args = parser.parse_args()
    if not args.lupdate:
        parser.error('provide --lupdate or add Qt Linguist tools to PATH')
    count = refresh_catalogue(args.lupdate, args.ts)
    print(f'Extracted {count} first-party source files.')


if __name__ == '__main__':
    main()
