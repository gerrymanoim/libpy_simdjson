from pathlib import Path

import pytest

import libpy_simdjson as simdjson

JSON_FIXTURES_DIR = Path(__file__).parent / "jsonexamples"


@pytest.mark.parametrize(
    "test_path",
    list(JSON_FIXTURES_DIR.glob("**/*.json")),
)
def test_load_file(test_path):
    simdjson.load(test_path)
