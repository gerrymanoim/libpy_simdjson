# Libpy Simdjson

Python bindings for simdjson using libpy. 

```
In [1]: from libpy_simdjson import simdjson

In [2]: parser = simdjson.Simdjson()

In [3]: parser.load(b"twitter.json")
```

## JSON Pointer

We support JSON pointer via the at() method:

```
In [1]: from libpy_simdjson import simdjson

In [2]: parser = simdjson.Simdjson()

In [3]: parser.load("json_pointer.json")

In [4]: parser.at(b"0/tire_pressure/1")
Out[4]: 39.9
```