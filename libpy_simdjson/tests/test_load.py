from pathlib import Path

import pytest

from libpy_simdjson import simdjson

JSON_FIXTURES_DIR = Path(__file__).parent / "jsonexamples"


@pytest.mark.parametrize("test_path", list(JSON_FIXTURES_DIR.glob("**/*.json")))
def test_load_file(test_path):
    simdjson.load(bytes(test_path))
