# Libpy Simdjson

Python bindings for simdjson using libpy. 

```
In [1]: from libpy_simdjson import simdjson

In [2]: parser = simdjson.Simdjson()

In [3]: parser.load(b"twitter.json")
```

## Fields 

Access fields via `[]`:

```
In [1]: from libpy_simdjson import simdjson

In [2]: parser = simdjson.Simdjson()

In [3]: parser.load(b"small.json")

In [4]: parser[b"Image"]
Out[4]:
{b'Width': 800,
 b'Height': 600,
 b'Title': b'View from 15th Floor',
 b'Thumbnail': {b'Url': b'http://www.example.com/image/481989943',
  b'Height': 125,
  b'Width': 100},
 b'Animated': False,
 b'IDs': [116, 943, 234, 38793]}
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

## TODO

- [X] iterators
   - [X] object
   - [X] arrays
   - Note: Right now when iterate we convert everything automatically to python objects. Should we be smarter? Or is that not expected?
- [X] as dict methods for objects 
- [X] keys methods for objects
- [ ] values methods for objects
- [X] len methods
   - [X] objects
   - [X] arrays
- [ ] `count` for array
- [ ] `index` for array
- [ ] clean up exceptions
- [ ] clean up code
- [ ] fix names
- [ ] fix directory structures
- [ ] benchmarks
- [ ] tests/CI/publish
- [ ] a real README
- [ ] Package a release
- [ ] string padding?
- [ ] extra functions
   - [ ] validate_utf8
   - [ ] load_many/parse_many
   - [ ] load_many/parse_many with threads?
   - [ ] case insensitive at?

