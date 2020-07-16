# libpy simdjson

![On Master Merge](https://github.com/gerrymanoim/libpy_simdjson/workflows/On%20Master%20Merge/badge.svg)

Python bindings for [simdjson](https://github.com/simdjson/simdjson/) using [libpy](https://github.com/quantopian/libpy).

## Requirements

- OS: macOS>10.15, linux.
- Compiler: gcc>=9, clang >= 10 (C++17 code)
- Python: libpy>=0.2.3, numpy.

## Install

`pip install libpy-simdjson`

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

## Benchmarks

**Note** - unlike most other python JSON parsers, `libpy_simdjson` will, by design, avoid converting to native python types until as late as possible, providing you with `Object` and `Array` objects instead. `libpy` allows you to work with these proxy objects as if they were actual python objects without incurring the cost of object conversion until actually needed. Because the C++ `simdjson` library is so effficient, converting to Python objects is by far the slowest part of parsing, so we strive to do this as late and on as few fields as possible.

See the (still WIP) "overhead over python dict access" benchmarks for object conversion overhead.

For the sake of comparison, we also benchmark a full Python object conversion in `libpy_simdjson_as_py_obj` though this is very much not the intended use case.


```

---------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/canada.json': 6 tests ----------------------------------------------
Name (time in ms)                                       Min                Max               Mean            StdDev             Median                IQR            Outliers       OPS            Rounds  Iterations
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path0-libpy_simdjson-loads]      3.4478 (1.0)      10.1485 (1.0)       4.0615 (1.0)      0.6386 (1.0)       3.9595 (1.0)       0.3985 (1.0)           8;6  246.2156 (1.0)         149           1
test_benchmark_load[path0-orjson-loads]             14.7421 (4.28)     31.9980 (3.15)     21.1131 (5.20)     4.7609 (7.45)     21.8631 (5.52)      8.2455 (20.69)        23;0   47.3639 (0.19)         61           1
test_benchmark_load[path0-pysimdjson-loads]         15.5617 (4.51)     30.0839 (2.96)     22.2207 (5.47)     4.3227 (6.77)     23.6153 (5.96)      8.4906 (21.31)        12;0   45.0031 (0.18)         30           1
test_benchmark_load[path0-ujson-loads]              20.0784 (5.82)     37.2904 (3.67)     27.4904 (6.77)     4.6357 (7.26)     27.7301 (7.00)      8.1542 (20.46)         9;0   36.3763 (0.15)         26           1
test_benchmark_load[path0-rapidjson-loads]          44.7989 (12.99)    69.9204 (6.89)     53.8819 (13.27)    6.2806 (9.83)     54.5078 (13.77)    10.5220 (26.40)         6;0   18.5591 (0.08)         20           1
test_benchmark_load[path0-python_json-loads]        45.6048 (13.23)    58.9150 (5.81)     52.6407 (12.96)    4.2356 (6.63)     53.2421 (13.45)     7.6745 (19.26)         9;0   18.9967 (0.08)         21           1
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


------------------------------------------------------ benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/citm_catalog.json': 6 tests -------------------------------------------------------
Name (time in us)                                           Min                    Max                   Mean                StdDev                 Median                   IQR            Outliers       OPS            Rounds  Iterations
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path3-libpy_simdjson-loads]        973.0290 (1.0)       1,696.1500 (1.0)       1,106.7939 (1.0)         70.3023 (1.0)       1,096.5330 (1.0)         55.0015 (1.0)        107;65  903.5106 (1.0)         496           1
test_benchmark_load[path3-orjson-loads]              6,271.9950 (6.45)     18,752.0820 (11.06)     9,199.1053 (8.31)     3,332.8687 (47.41)     7,502.8330 (6.84)     3,940.9760 (71.65)        32;1  108.7062 (0.12)        128           1
test_benchmark_load[path3-pysimdjson-loads]          7,448.6360 (7.66)     21,308.7680 (12.56)    10,668.5839 (9.64)     3,595.1711 (51.14)     8,919.9800 (8.13)     1,307.4410 (23.77)       24;24   93.7332 (0.10)        102           1
test_benchmark_load[path3-ujson-loads]               7,774.9390 (7.99)     17,898.5500 (10.55)    10,364.6843 (9.36)     3,222.6374 (45.84)     8,751.2690 (7.98)     1,562.5480 (28.41)       26;26   96.4815 (0.11)        115           1
test_benchmark_load[path3-python_json-loads]        11,643.7470 (11.97)    23,959.7150 (14.13)    15,714.9961 (14.20)    3,806.9531 (54.15)    13,973.4170 (12.74)    6,292.6375 (114.41)       12;0   63.6335 (0.07)         41           1
test_benchmark_load[path3-rapidjson-loads]          13,983.3210 (14.37)    27,216.4270 (16.05)    17,630.6505 (15.93)    4,016.1918 (57.13)    15,564.2690 (14.19)    2,136.0153 (38.84)       15;15   56.7194 (0.06)         65           1
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/github_events.json': 6 tests ------------------------------------------------
Name (time in us)                                        Min                   Max                Mean              StdDev              Median                IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path2-libpy_simdjson-loads]      31.8010 (1.0)      5,766.8830 (6.11)      37.5110 (1.0)       59.9135 (1.24)      37.0010 (1.0)       0.2000 (1.0)        9;3552       26.6588 (1.0)        9200           1
test_benchmark_load[path2-orjson-loads]             229.6080 (7.22)     4,736.2550 (5.02)     266.4467 (7.10)      94.5404 (1.96)     266.1090 (7.19)     40.8512 (204.26)      56;75        3.7531 (0.14)       3243           1
test_benchmark_load[path2-pysimdjson-loads]         291.1090 (9.15)     1,112.7370 (1.18)     340.7878 (9.09)      48.2980 (1.0)      336.6110 (9.10)     33.8510 (169.25)     214;48        2.9344 (0.11)       2187           1
test_benchmark_load[path2-ujson-loads]              300.1100 (9.44)     4,311.1400 (4.57)     342.2005 (9.12)      93.3709 (1.93)     346.5110 (9.36)     50.4020 (252.01)      26;36        2.9223 (0.11)       2258           1
test_benchmark_load[path2-rapidjson-loads]          379.0120 (11.92)    4,312.8390 (4.57)     518.6963 (13.83)    117.7450 (2.44)     507.6160 (13.72)    51.0268 (255.13)      37;40        1.9279 (0.07)       1717           1
test_benchmark_load[path2-python_json-loads]        382.2120 (12.02)      943.6300 (1.0)      439.8152 (11.72)     50.1689 (1.04)     443.7140 (11.99)    82.9020 (414.51)     665;18        2.2737 (0.09)       1894           1
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


-------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/mesh.json': 6 tests --------------------------------------------------------
Name (time in us)                                          Min                    Max                  Mean                StdDev                Median                 IQR            Outliers       OPS            Rounds  Iterations
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path4-libpy_simdjson-loads]       993.7280 (1.0)       2,153.3610 (1.0)      1,113.0914 (1.0)        125.6128 (1.0)      1,122.9820 (1.0)      147.0050 (1.0)         64;16  898.3988 (1.0)         898           1
test_benchmark_load[path4-pysimdjson-loads]         3,019.2900 (3.04)     13,713.0090 (6.37)     3,958.4115 (3.56)     1,763.1884 (14.04)    3,619.4070 (3.22)     300.4090 (2.04)        10;14  252.6266 (0.28)        226           1
test_benchmark_load[path4-orjson-loads]             3,075.6900 (3.10)     12,985.8830 (6.03)     4,371.5742 (3.93)     1,528.5850 (12.17)    4,067.1200 (3.62)     444.3125 (3.02)        10;14  228.7506 (0.25)        240           1
test_benchmark_load[path4-ujson-loads]              3,947.6150 (3.97)     13,696.0010 (6.36)     4,954.1335 (4.45)     1,521.1764 (12.11)    4,690.3375 (4.18)     390.0120 (2.65)          8;9  201.8516 (0.22)        218           1
test_benchmark_load[path4-python_json-loads]        7,593.0170 (7.64)     19,002.5420 (8.82)     9,068.5910 (8.15)     1,944.1363 (15.48)    8,763.6505 (7.80)     649.6190 (4.42)          5;5  110.2707 (0.12)        122           1
test_benchmark_load[path4-rapidjson-loads]          8,291.5380 (8.34)     19,017.8470 (8.83)     9,628.5255 (8.65)     1,797.5745 (14.31)    9,276.3670 (8.26)     872.3250 (5.93)          4;4  103.8581 (0.12)        102           1
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


-------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/twitter.json': 6 tests -------------------------------------------------------
Name (time in us)                                          Min                    Max                  Mean                StdDev                Median                 IQR            Outliers         OPS            Rounds  Iterations
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path1-libpy_simdjson-loads]       374.2130 (1.0)      10,169.1400 (1.0)        445.6502 (1.0)        237.7491 (1.0)        443.3150 (1.0)       66.3020 (1.0)         19;29  2,243.9125 (1.0)        1790           1
test_benchmark_load[path1-orjson-loads]             2,788.1970 (7.45)     11,687.4110 (1.15)     3,351.3276 (7.52)     1,117.1151 (4.70)     3,198.9625 (7.22)     351.0120 (5.29)        10;12    298.3892 (0.13)        294           1
test_benchmark_load[path1-ujson-loads]              3,312.1150 (8.85)     12,571.4370 (1.24)     3,973.3347 (8.92)     1,221.4127 (5.14)     3,805.8815 (8.59)     447.3170 (6.75)          7;9    251.6778 (0.11)        258           1
test_benchmark_load[path1-pysimdjson-loads]         3,586.0280 (9.58)     18,704.8590 (1.84)     4,553.9661 (10.22)    1,772.5065 (7.46)     4,182.3480 (9.43)     331.1612 (4.99)         7;17    219.5888 (0.10)        169           1
test_benchmark_load[path1-python_json-loads]        4,573.6530 (12.22)    13,900.1650 (1.37)     5,396.5765 (12.11)    1,236.4753 (5.20)     5,222.7750 (11.78)    554.0430 (8.36)          6;7    185.3027 (0.08)        189           1
test_benchmark_load[path1-rapidjson-loads]          5,447.2870 (14.56)    16,226.5570 (1.60)     6,506.3766 (14.60)    1,495.7694 (6.29)     6,322.1140 (14.26)    544.9407 (8.22)          6;7    153.6954 (0.07)        165           1
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



------------------------------------------------------------------------------------------- benchmark 'Random attribute access': 2 tests -------------------------------------------------------------------------------------------
Name (time in ns)                                  Min                     Max                  Mean                StdDev                Median                 IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_at[python_json-loads]          700.0000 (1.0)      102,901.0000 (1.0)        965.7227 (1.0)      1,212.7318 (1.0)        900.0000 (1.0)      100.0000 (1.0)     477;11190    1,035.4939 (1.0)      121952           1
test_benchmark_at[libpy_simdjson-loads]     1,100.0000 (1.57)     186,701.0000 (1.81)     1,703.0050 (1.76)     1,991.5598 (1.64)     1,500.0000 (1.67)     300.0000 (3.00)     570;2422      587.1973 (0.57)      80001           1
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Legend:
  Outliers: 1 Standard Deviation from Mean; 1.5 IQR (InterQuartile Range) from 1st Quartile and 3rd Quartile.
  OPS: Operations Per Second, computed as 1 / Mean
================== 71 passed, 1 xfailed, 1 warning in 29.65s ===================
```
