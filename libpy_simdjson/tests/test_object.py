import libpy_simdjson as simdjson


def test_as_dict(object_element, py_object_element):
    actual_dict = object_element.as_dict()
    assert actual_dict == py_object_element


def test_keys(object_element):
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
