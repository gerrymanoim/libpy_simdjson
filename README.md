# libpy Simdjson

**Status**: Working Alpha

Python bindings for [simdjson](https://github.com/simdjson/simdjson/) using [libpy](https://github.com/quantopian/libpy).

## Usage

```python
from pathlib import Path
import libpy_simdjson as json
```


```python
doc = json.load(Path("twitter.json"))
# or json.load(b"twitter.json")
# or json.load("twitter.json")
# we also support `loads` for strings. 
```

`doc` is an `Object`. Objects act as python dicts with special methods. 


```python
isinstance(doc, json.Object)
```




    True



We can grab keys, get the length, grab items, and access specific keys:


```python
len(doc)
```




    2




```python
doc.keys()
```




    [b'statuses', b'search_metadata']




```python
doc[b'search_metadata'].items()
```




    [(b'completed_in', 0.087),
     (b'max_id', 505874924095815700),
     (b'max_id_str', b'505874924095815681'),
     (b'next_results',
      b'?max_id=505874847260352512&q=%E4%B8%80&count=100&include_entities=1'),
     (b'query', b'%E4%B8%80'),
     (b'refresh_url',
      b'?since_id=505874924095815681&q=%E4%B8%80&include_entities=1'),
     (b'count', 100),
     (b'since_id', 0),
     (b'since_id_str', b'0')]



If you every want an actual python dictionary, use `as_dict`:


```python
doc[b'search_metadata'].as_dict()
```




    {b'completed_in': 0.087,
     b'max_id': 505874924095815700,
     b'max_id_str': b'505874924095815681',
     b'next_results': b'?max_id=505874847260352512&q=%E4%B8%80&count=100&include_entities=1',
     b'query': b'%E4%B8%80',
     b'refresh_url': b'?since_id=505874924095815681&q=%E4%B8%80&include_entities=1',
     b'count': 100,
     b'since_id': 0,
     b'since_id_str': b'0'}



However, we also support [JSON Pointer](https://tools.ietf.org/html/rfc6901) sytnax via `at`. This will be much faster if you know what you're looking for:


```python
doc.at(b"statuses/50/created_at")
```




    b'Sun Aug 31 00:29:04 +0000 2014'




```python
doc.at(b"statuses/50/text").decode()
```




    'RT @Ang_Angel73: 逢坂「くっ…僕の秘められし右目が…！」\n一同「……………。」'



Let's look at `statuses`


```python
statuses = doc[b'statuses']
```

`statuses` is an `Array`. Arrays act like python lists with special methods.

Note: `statuses` and `doc` share a single parser instance. We cannot parse a new document while these objects are alive (though we can create new parsers via `libpy_simdjson.Parser.load`.


```python
isinstance(statuses, json.Array)
```




    True



Arrays support length, indexing, iteration:


```python
len(statuses)
```




    100




```python
statuses[0][b'text'].decode()
```




    '@aym0566x \n\n名前:前田あゆみ\n第一印象:なんか怖っ！\n今の印象:とりあえずキモい。噛み合わない\n好きなところ:ぶすでキモいとこ😋✨✨\n思い出:んーーー、ありすぎ😊❤️\nLINE交換できる？:あぁ……ごめん✋\nトプ画をみて:照れますがな😘✨\n一言:お前は一生もんのダチ💖'




```python
for status in statuses:
    # this is a bad example but you get the picture
    if status[b'id'] % 2 == 0:
        print(status[b"text"].decode())
        break
else:
    print("no even ids?")
```

    @aym0566x 
    
    名前:前田あゆみ
    第一印象:なんか怖っ！
    今の印象:とりあえずキモい。噛み合わない
    好きなところ:ぶすでキモいとこ😋✨✨
    思い出:んーーー、ありすぎ😊❤️
    LINE交換できる？:あぁ……ごめん✋
    トプ画をみて:照れますがな😘✨
    一言:お前は一生もんのダチ💖


If you need to you can convert and Array to a list using `as_list`:


```python
statuses.as_list()[1][b'metadata']
```




    {b'result_type': b'recent', b'iso_language_code': b'ja'}



However, just like for Objects, we support JSON Pointers via `at`, which is much faster:


```python
statuses.at(b"33/created_at")
```

    b'Sun Aug 31 00:29:06 +0000 2014'


## TODO

- [X] iterators
   - [X] object
     - [ ] Note iterator for object is really `object.items()` which does not match python. todo to fix.
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
- [ ] const everything that one might const
- [ ] clean up exceptions
  - [X] Change ValueError to IndexError for OOB Array indexing
- [ ] clean up code
- [ ] fix names
- [ ] fix directory structures
- [ ] benchmarks
- [ ] Other compilter flags?
- [ ] tests/CI/publish
- [ ] a real README
- [ ] Package a release
- [ ] string padding?
- [ ] extra functions
   - [ ] validate_utf8
   - [ ] load_many/parse_many
   - [ ] load_many/parse_many with threads?
   - [ ] case insensitive at?
- [ ] Add reprs to Object and Array
- [ ] Support negative indexing in Array
- [ ] Expose versions of libpy_simdjson and simdjson
