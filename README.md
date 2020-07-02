# Libpy Simdjson

Python bindings for simdjson using libpy. 

```
In [1]: from libpy_simdjson import simdjson

In [2]: parser = simdjson.Simdjson()

In [3]: parser.load(b"twitter.json")
```