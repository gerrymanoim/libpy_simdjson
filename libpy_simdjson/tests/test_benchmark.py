import random

from json import loads as json_loads
from pathlib import Path

import pytest

from orjson import loads as orjson_loads
from ujson import loads as ujson_loads
from rapidjson import loads as rapidjson_loads
from simdjson import loads as pysimdjson_loads
from simdjson import Parser
from libpy_simdjson import loads as libpy_simdjson_loads

from libpy_simdjson import Array


JSON_FIXTURES_DIR = Path(__file__).parent / "jsonexamples"


def libpy_simdjson_as_py_obj(input):
    doc = libpy_simdjson_loads(input)
    if isinstance(doc, Array):
        doc.as_list()
    else:
        doc.as_dict()


pysimdjson_parser = Parser()


@pytest.mark.slow
@pytest.mark.parametrize(
    ["group", "func"],
    [
        ("pysimdjson_as_py_obj", pysimdjson_loads),
        ("pysimdjson", pysimdjson_parser.parse),
        ("orjson", orjson_loads),
        ("ujson", ujson_loads),
        ("rapidjson", rapidjson_loads),
        ("python_json", json_loads),
        ("libpy_simdjson", libpy_simdjson_loads),
        ("libpy_simdjson_as_py_obj", libpy_simdjson_as_py_obj),
    ],
)
@pytest.mark.parametrize(
    "path",
    [
        JSON_FIXTURES_DIR / "canada.json",
        JSON_FIXTURES_DIR / "twitter.json",
        JSON_FIXTURES_DIR / "github_events.json",
        JSON_FIXTURES_DIR / "citm_catalog.json",
        JSON_FIXTURES_DIR / "mesh.json",
    ],
)
def test_benchmark_load(group, func, path, benchmark):
    benchmark.group = f"Load {path}"
    benchmark.extra_info["group"] = group

    with path.open('rb') as f:
        content = f.read()
        benchmark(func, content)


@pytest.mark.parametrize(
    ["group", "read_func"],
    [
        ("python_json", json_loads),
        ("libpy_simdjson", libpy_simdjson_loads),
    ],
)
def test_benchmark_at(group, read_func, benchmark):
    benchmark.group = "Random attribute access"
    benchmark.extra_info["group"] = group

    random.seed(999)

    def simd_test_func(doc):
        selection = random.randrange(100)
        at_str = f"statuses/{selection}/user/id".encode()
        doc.at(at_str)

    def py_test_func(doc):
        selection = random.randrange(100)
        doc["statuses"][selection]["user"]["id"]

    if group == "libpy_simdjson":
        bench_func = simd_test_func
    elif group == "python_json":
        bench_func = py_test_func
    else:
        raise ValueError("unknown group for direct access test")

    json_doc = JSON_FIXTURES_DIR / "twitter.json"
    with json_doc.open('rb') as f:
        content = f.read()
        doc = read_func(content)
        benchmark(bench_func, doc)


@pytest.mark.parametrize(
    ["group", "read_func"],
    [
        ("python_json", json_loads),
        ("libpy_simdjson", libpy_simdjson_loads),
    ],
)
def test_benchmark_list_access(group, read_func, benchmark):
    benchmark.group = "Random list access"
    benchmark.extra_info["group"] = group

    random.seed(999)

    def simd_test_func(doc):
        selection = random.randrange(10_000)
        doc[selection]

    def py_test_func(doc):
        selection = random.randrange(10_000)
        doc[selection]

    if group == "libpy_simdjson":
        bench_func = simd_test_func
    elif group == "python_json":
        bench_func = py_test_func
    else:
        raise ValueError("unknown group for direct access test")

    json_doc = JSON_FIXTURES_DIR / "numbers.json"
    with json_doc.open('rb') as f:
        content = f.read()
        doc = read_func(content)
        benchmark(bench_func, doc)

