import json
from pathlib import Path

import pytest

from libpy_simdjson import simdjson

JSON_FIXTURES_DIR = Path(__file__).parent / "jsonexamples"


@pytest.fixture
def object_element():
    file_path = JSON_FIXTURES_DIR / "small/smalldemo.json"
    return simdjson.load(bytes(file_path))


@pytest.fixture
def py_object_element():
    return {
        b"Width": 800,
        b"Height": 600,
        b"Title": b"View from my room",
        b"Url": b"http://ex.com/img.png",
        b"Private": False,
        b"Thumbnail": {b"Url": b"http://ex.com/th.png", b"Height": 125, b"Width": 100},
        b"array": [116, 943, 234],
        b"Owner": None,
    }


@pytest.fixture
def array_element():
    file_path = JSON_FIXTURES_DIR / "small/smalllist.json"
    return simdjson.load(bytes(file_path))


@pytest.fixture
def py_array_element():
    file_path = JSON_FIXTURES_DIR / "small/smalllist.json"
    return json.load(file_path.open())
