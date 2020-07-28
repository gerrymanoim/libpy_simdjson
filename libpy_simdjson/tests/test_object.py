from pathlib import Path

import libpy_simdjson as simdjson


JSON_FIXTURES_DIR = Path(__file__).parent / "jsonexamples"


def test_as_dict(object_element, py_object_element):
    actual_dict = object_element.as_dict()
    assert actual_dict == py_object_element


def test_keys(object_element, py_object_element):
    keys = object_element.keys()
    assert keys == [
        b"Width",
        b"Height",
        b"Title",
        b"Url",
        b"Private",
        b"Thumbnail",
        b"array",
        b"Owner",
    ]
    assert set(object_element.keys()) == set(py_object_element.keys())


def test_values(object_element, py_object_element):
    values = object_element.values()
    for n, k in enumerate(object_element):
        expected = object_element[k]
        if isinstance(expected, simdjson.Object):
            expected = expected.as_dict()
        elif isinstance(expected, simdjson.Array):
            expected = expected.as_list()

        assert values[n] == expected
        assert values[n] == py_object_element[k]


def test_len(object_element):
    assert len(object_element) == 8


def test_iteration(object_element, py_object_element):
    for cpp, py in zip(object_element, py_object_element):
        assert cpp == py


def test_items(object_element, py_object_element):
    for cpp, py in zip(object_element.items(), py_object_element.items()):
        assert cpp == py


def test_mapping(object_element):
    assert object_element[b"Width"] == 800


def test_at(object_element):
    assert object_element.at(b"Width") == 800
    assert isinstance(object_element.at(b"array"), simdjson.Array)
    assert object_element.at(b"array/0") == 116


def get_new_object_element(object_element):
    return object_element


def test_equality():
    file_path = JSON_FIXTURES_DIR / "small/smalldemo.json"
    elem_1 = simdjson.load(bytes(file_path))
    elem_2 = simdjson.load(bytes(file_path))

    assert elem_1 == elem_2
