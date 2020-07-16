# libpy simdjson

![On Master Merge](https://github.com/gerrymanoim/libpy_simdjson/workflows/On%20Master%20Merge/badge.svg)

Python bindings for [simdjson](https://github.com/simdjson/simdjson/) using [libpy](https://github.com/quantopian/libpy).

## Requirements

- OS: macOS>10.15, linux.
- Compiler: gcc>=9, clang >= 10 (C++17 code)
- Python: libpy>=0.2.3, numpy.

## Install

`pip install libpy-simdjson`

**Note**: The installation of `libpy` (required by `libpy_simdjson`) will use the `python` executable to figure out information about your environment. If you are not using a virtual environment or `python` does not point to the Python installation you want to use (checked with `which python` and `python --version`) you must point to your Python executable using the `PYTHON` environment variable, i.e. `PYTHON=python3 make` or `PYTHON=python3 pip3 install libpy`. Additionally, make sure that your `CC` and `CXX` environment variables point to the correct compilers.


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

------------------------------------------------------------ benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/canada.json': 7 tests -------------------------------------------------------------
Name (time in ms)                                                                    Min                Max               Mean            StdDev             Median                IQR            Outliers       OPS            Rounds  Iterations
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path0-libpy_simdjson-loads]                                   3.8537 (1.0)       5.9011 (1.0)       4.4181 (1.0)      0.3282 (1.0)       4.3406 (1.0)       0.3466 (1.0)          36;6  226.3392 (1.0)         155           1
test_benchmark_load[path0-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     14.0344 (3.64)     29.2145 (4.95)     20.5918 (4.66)     4.8219 (14.69)    22.6632 (5.22)      9.3498 (26.98)        18;0   48.5630 (0.21)         41           1
test_benchmark_load[path0-pysimdjson-loads]                                      14.2690 (3.70)     33.3402 (5.65)     23.3360 (5.28)     5.7488 (17.52)    25.6553 (5.91)     10.2900 (29.69)        16;0   42.8522 (0.19)         39           1
test_benchmark_load[path0-orjson-loads]                                          17.6921 (4.59)     34.5362 (5.85)     25.1072 (5.68)     5.4175 (16.51)    28.0433 (6.46)     10.6798 (30.81)        16;0   39.8293 (0.18)         37           1
test_benchmark_load[path0-ujson-loads]                                           25.2277 (6.55)     39.2225 (6.65)     32.0272 (7.25)     4.7504 (14.47)    34.2962 (7.90)      8.5241 (24.59)        11;0   31.2234 (0.14)         27           1
test_benchmark_load[path0-python_json-loads]                                     53.2037 (13.81)    69.0170 (11.70)    60.9342 (13.79)    5.2097 (15.87)    62.8423 (14.48)     9.2608 (26.72)         5;0   16.4112 (0.07)         15           1
test_benchmark_load[path0-rapidjson-loads]                                       55.2075 (14.33)    72.6798 (12.32)    64.4932 (14.60)    5.8893 (17.94)    67.0647 (15.45)    10.1668 (29.33)         7;0   15.5055 (0.07)         16           1
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



--------------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/citm_catalog.json': 7 tests ---------------------------------------------------------------------
Name (time in us)                                                                        Min                    Max                   Mean                StdDev                 Median                   IQR            Outliers       OPS            Rounds  Iterations
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path3-libpy_simdjson-loads]                                     903.7070 (1.0)       2,713.4210 (1.0)       1,104.2687 (1.0)        154.8287 (1.0)       1,068.0580 (1.0)        171.7010 (1.0)         84;12  905.5767 (1.0)         482           1
test_benchmark_load[path3-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]      7,072.7540 (7.83)     20,125.7550 (7.42)      9,936.1735 (9.00)     3,955.5863 (25.55)     7,994.1610 (7.48)       948.7070 (5.53)        26;26  100.6424 (0.11)        118           1
test_benchmark_load[path3-orjson-loads]                                           7,165.9570 (7.93)     17,959.2440 (6.62)     10,022.8959 (9.08)     3,528.7747 (22.79)     8,292.7665 (7.76)     1,144.8095 (6.67)        26;26   99.7716 (0.11)        116           1
test_benchmark_load[path3-pysimdjson-loads]                                       8,612.4690 (9.53)     19,956.9630 (7.35)     11,554.6483 (10.46)    3,679.3161 (23.76)     9,770.1790 (9.15)     1,354.6115 (7.89)        12;12   86.5453 (0.10)         53           1
test_benchmark_load[path3-ujson-loads]                                            9,407.2750 (10.41)    19,387.2540 (7.14)     11,980.5150 (10.85)    3,513.4523 (22.69)    10,306.5820 (9.65)     1,214.6855 (7.07)        12;12   83.4689 (0.09)         55           1
test_benchmark_load[path3-python_json-loads]                                     13,717.4070 (15.18)    27,073.1100 (9.98)     16,927.4024 (15.33)    3,853.8621 (24.89)    15,197.6180 (14.23)    1,684.1633 (9.81)        11;11   59.0758 (0.07)         51           1
test_benchmark_load[path3-rapidjson-loads]                                       15,754.2240 (17.43)    29,229.9300 (10.77)    18,797.9650 (17.02)    4,115.5486 (26.58)    16,836.3320 (15.76)    2,288.4678 (13.33)         7;7   53.1972 (0.06)         35           1
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



---------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/github_events.json': 7 tests ---------------------------------------------------------------
Name (time in us)                                                                     Min                   Max                Mean              StdDev              Median                 IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path2-libpy_simdjson-loads]                                   38.1000 (1.0)        710.9060 (1.0)       49.7735 (1.0)       17.2851 (1.0)       44.8010 (1.0)        6.0997 (1.0)      796;1680       20.0910 (1.0)        9243           1
test_benchmark_load[path2-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     193.4010 (5.08)       980.5080 (1.38)     241.4011 (4.85)      36.4210 (2.11)     234.0020 (5.22)      31.6000 (5.18)      363;157        4.1425 (0.21)       2739           1
test_benchmark_load[path2-orjson-loads]                                          235.2020 (6.17)     1,194.7110 (1.68)     291.7885 (5.86)      53.3101 (3.08)     279.2020 (6.23)      45.4010 (7.44)      323;140        3.4271 (0.17)       3198           1
test_benchmark_load[path2-ujson-loads]                                           302.5020 (7.94)     2,333.7190 (3.28)     387.0228 (7.78)      73.9336 (4.28)     365.9030 (8.17)      65.6505 (10.76)     289;117        2.5838 (0.13)       2404           1
test_benchmark_load[path2-pysimdjson-loads]                                      303.9030 (7.98)       763.0060 (1.07)     383.3152 (7.70)      54.9548 (3.18)     367.7030 (8.21)      58.0010 (9.51)      358;104        2.6088 (0.13)       2033           1
test_benchmark_load[path2-python_json-loads]                                     379.9030 (9.97)     7,045.3590 (9.91)     481.8427 (9.68)     179.3289 (10.37)    449.7030 (10.04)     79.2500 (12.99)       59;93        2.0754 (0.10)       1919           1
test_benchmark_load[path2-rapidjson-loads]                                       421.1030 (11.05)    1,260.2110 (1.77)     549.1410 (11.03)     86.9668 (5.03)     521.5540 (11.64)    103.2505 (16.93)      384;57        1.8210 (0.09)       1744           1
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




------------------------------------------------------------------------ benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/mesh.json': 7 tests ------------------------------------------------------------------------
Name (time in us)                                                                        Min                    Max                   Mean                StdDev                 Median                 IQR            Outliers       OPS            Rounds  Iterations
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path4-libpy_simdjson-loads]                                     992.0070 (1.0)       2,344.3170 (1.0)       1,220.6543 (1.0)        136.6343 (1.0)       1,194.3080 (1.0)      147.5503 (1.0)        167;26  819.2328 (1.0)         779           1
test_benchmark_load[path4-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]      3,047.8210 (3.07)     14,840.7060 (6.33)      4,070.1827 (3.33)     1,912.7254 (14.00)     3,666.2260 (3.07)     409.5040 (2.78)         9;15  245.6892 (0.30)        234           1
test_benchmark_load[path4-pysimdjson-loads]                                       3,153.6230 (3.18)     13,503.4030 (5.76)      4,037.8932 (3.31)     1,640.2764 (12.00)     3,692.7280 (3.09)     430.3540 (2.92)        11;14  247.6539 (0.30)        265           1
test_benchmark_load[path4-orjson-loads]                                           3,855.9290 (3.89)     18,425.5370 (7.86)      4,998.6408 (4.10)     1,824.2956 (13.35)     4,654.4340 (3.90)     488.3037 (3.31)         9;12  200.0544 (0.24)        231           1
test_benchmark_load[path4-ujson-loads]                                            4,935.4360 (4.98)     17,177.9270 (7.33)      6,066.1678 (4.97)     1,875.5811 (13.73)     5,727.5420 (4.80)     500.4540 (3.39)          2;5  164.8487 (0.20)         65           1
test_benchmark_load[path4-python_json-loads]                                      8,959.9650 (9.03)     20,947.7510 (8.94)     10,572.3323 (8.66)     2,075.1962 (15.19)    10,123.0740 (8.48)     705.3805 (4.78)          4;8   94.5865 (0.12)         97           1
test_benchmark_load[path4-rapidjson-loads]                                       10,145.3740 (10.23)    21,652.7590 (9.24)     11,664.0530 (9.56)     1,909.6563 (13.98)    11,377.8835 (9.53)     542.2050 (3.67)          3;6   85.7335 (0.10)         86           1
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



---------------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/twitter.json': 7 tests ----------------------------------------------------------------------
Name (time in us)                                                                       Min                    Max                  Mean                StdDev                Median                 IQR            Outliers         OPS            Rounds  Iterations
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path1-libpy_simdjson-loads]                                    393.3030 (1.0)       9,689.1860 (1.0)        505.1563 (1.0)        270.8993 (1.0)        473.5040 (1.0)       85.2253 (1.0)         12;52  1,979.5852 (1.0)        1253           1
test_benchmark_load[path1-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     2,312.6200 (5.88)     13,150.9150 (1.36)     2,984.2530 (5.91)     1,327.4901 (4.90)     2,748.9240 (5.81)     329.2530 (3.86)         6;13    335.0922 (0.17)        308           1
test_benchmark_load[path1-orjson-loads]                                          3,044.1280 (7.74)     14,742.8370 (1.52)     3,771.8208 (7.47)     1,338.7861 (4.94)     3,533.0320 (7.46)     293.7023 (3.45)         7;23    265.1239 (0.13)        285           1
test_benchmark_load[path1-ujson-loads]                                           3,817.8350 (9.71)     13,823.1250 (1.43)     4,708.7464 (9.32)     1,306.0134 (4.82)     4,510.9910 (9.53)     371.7030 (4.36)          5;9    212.3708 (0.11)        206           1
test_benchmark_load[path1-pysimdjson-loads]                                      3,934.6370 (10.00)    18,011.0690 (1.86)     4,774.5767 (9.45)     1,498.8542 (5.53)     4,527.5420 (9.56)     320.9030 (3.77)         4;11    209.4427 (0.11)        159           1
test_benchmark_load[path1-python_json-loads]                                     4,894.4440 (12.44)    15,869.4420 (1.64)     5,915.9692 (11.71)    1,304.7454 (4.82)     5,750.9510 (12.15)    420.6040 (4.94)          3;7    169.0340 (0.09)        170           1
test_benchmark_load[path1-rapidjson-loads]                                       6,177.5560 (15.71)    17,475.8570 (1.80)     7,176.0166 (14.21)    1,494.8211 (5.52)     6,892.7610 (14.56)    375.3782 (4.40)         5;11    139.3531 (0.07)        137           1
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------- benchmark 'Random attribute access': 2 tests -------------------------------------------------------------------------------------------
Name (time in ns)                                  Min                     Max                  Mean                StdDev                Median                 IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_at[python_json-loads]          800.0000 (1.0)      249,102.0001 (1.0)      1,194.0692 (1.0)      2,040.4680 (1.0)      1,100.0000 (1.0)      100.0000 (1.0)      741;5864      837.4724 (1.0)      116280           1
test_benchmark_at[libpy_simdjson-loads]     1,300.0000 (1.63)     293,402.0000 (1.18)     2,090.2715 (1.75)     2,798.7105 (1.37)     1,800.0000 (1.64)     300.0000 (3.00)     919;1502      478.4068 (0.57)      78126           1
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Legend:
  Outliers: 1 Standard Deviation from Mean; 1.5 IQR (InterQuartile Range) from 1st Quartile and 3rd Quartile.
  OPS: Operations Per Second, computed as 1 / Mean
```
