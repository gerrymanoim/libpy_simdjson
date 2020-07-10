def test_as_list(array_element, py_array_element):
    actual_list = array_element.as_list()
    assert actual_list == py_array_element


def test_len(array_element):
    assert len(array_element) == 20


def test_iteration(array_element, py_array_element):
    for cpp, py in zip(array_element, py_array_element):
        assert cpp == py


def test_indexing(array_element):
    assert array_element[5] == 5


def test_at(array_element):
    assert array_element.at(b"5") == 5
