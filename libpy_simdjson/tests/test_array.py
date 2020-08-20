from pathlib import Path

import libpy_simdjson as simdjson


JSON_FIXTURES_DIR = Path(__file__).parent / "jsonexamples"


def test_as_list(array_element, py_array_element):
    actual_list = array_element.as_list()
    assert actual_list == py_array_element


def test_len(array_element):
    assert len(array_element) == 24


def test_iteration(array_element, py_array_element):
    for cpp, py in zip(array_element, py_array_element):
        assert cpp == py


def test_indexing(array_element):
    assert array_element[5] == 5


def test_at(array_element):
    assert array_element.at_pointer(b"/5") == 5


def test_equality():
    file_path = JSON_FIXTURES_DIR / "small/smalllist.json"
    elem_1 = simdjson.load(bytes(file_path))
    elem_2 = simdjson.load(bytes(file_path))

    assert elem_1 == elem_2


def test_count_specialzied(array_element):
    assert array_element.count(19) == 5


def test_count_generic(heterogeneous_array_element):
    assert heterogeneous_array_element.count(True) == 1000
    assert heterogeneous_array_element.count(None) == 1000


def test_index_specialized(array_element):
    assert array_element.index(19) == 19


def test_index_generic(heterogeneous_array_element):
    assert heterogeneous_array_element.index(True) == 1
    assert heterogeneous_array_element.index(None) == 2
