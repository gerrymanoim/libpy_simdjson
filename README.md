# libpy simdjson

![On Master Merge](https://github.com/gerrymanoim/libpy_simdjson/workflows/On%20Master%20Merge/badge.svg)
[![PyPI version](https://badge.fury.io/py/libpy-simdjson.svg)](https://badge.fury.io/py/libpy-simdjson)

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



However, we also support [JSON Pointer](https://tools.ietf.org/html/rfc6901) sytnax via `at_pointer`. This will be much faster if you know what you're looking for:


```python
doc.at_pointer(b"/statuses/50/created_at")
```




    b'Sun Aug 31 00:29:04 +0000 2014'




```python
doc.at_pointer(b"/statuses/50/text").decode()
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



However, just like for Objects, we support JSON Pointers via `at_pointer`, which is much faster:


```python
statuses.at_pointer(b"/33/created_at")
```

    b'Sun Aug 31 00:29:06 +0000 2014'

## Benchmarks

**Note** - unlike most other python JSON parsers, `libpy_simdjson` will, by design, avoid converting to native python types until as late as possible, providing you with `Object` and `Array` objects instead. `libpy` allows you to work with these proxy objects as if they were actual python objects without incurring the cost of object conversion until actually needed. Because the C++ `simdjson` library is so effficient, converting to Python objects is by far the slowest part of parsing, so we strive to do this as late and on as few fields as possible.

See the (still WIP) "overhead over python dict access" benchmarks for object conversion overhead.

For the sake of comparison, we also benchmark a full Python object conversion in `libpy_simdjson_as_py_obj` though this is very much not the intended use case.


```

------------------------------------------------------------ benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/canada.json': 8 tests -------------------------------------------------------------
Name (time in ms)                                                                    Min                Max               Mean            StdDev             Median                IQR            Outliers       OPS            Rounds  Iterations
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path0-libpy_simdjson-loads]                                   3.5037 (1.0)      10.9591 (1.0)       4.3319 (1.0)      0.6166 (1.0)       4.2607 (1.0)       0.2292 (1.0)         11;15  230.8454 (1.0)         162           1
test_benchmark_load[path0-pysimdjson-parse]                                       3.6885 (1.05)     11.1368 (1.02)      4.4029 (1.02)     0.7765 (1.26)      4.2611 (1.00)      0.5017 (2.19)          8;5  227.1254 (0.98)        118           1
test_benchmark_load[path0-pysimdjson_as_py_obj-loads]                            13.5282 (3.86)     37.4092 (3.41)     24.2264 (5.59)     6.5177 (10.57)    27.0374 (6.35)     11.9384 (52.08)        16;0   41.2773 (0.18)         40           1
test_benchmark_load[path0-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     13.5544 (3.87)     44.5382 (4.06)     22.4503 (5.18)     7.1879 (11.66)    25.0067 (5.87)     12.0174 (52.43)        12;0   44.5427 (0.19)         35           1
test_benchmark_load[path0-orjson-loads]                                          16.1693 (4.61)     37.2228 (3.40)     25.2505 (5.83)     6.8427 (11.10)    27.4105 (6.43)     12.5579 (54.79)        19;0   39.6032 (0.17)         41           1
test_benchmark_load[path0-ujson-loads]                                           22.0310 (6.29)     45.6815 (4.17)     32.3445 (7.47)     7.0874 (11.49)    35.0020 (8.22)     12.6422 (55.15)        12;0   30.9171 (0.13)         27           1
test_benchmark_load[path0-python_json-loads]                                     49.6505 (14.17)    72.4977 (6.62)     62.0533 (14.32)    7.2277 (11.72)    64.0998 (15.04)    12.7639 (55.68)         8;0   16.1152 (0.07)         19           1
test_benchmark_load[path0-rapidjson-loads]                                       50.3836 (14.38)    76.2291 (6.96)     61.1768 (14.12)    7.7522 (12.57)    63.4637 (14.90)    12.2982 (53.65)         6;0   16.3461 (0.07)         17           1
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



--------------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/citm_catalog.json': 8 tests ---------------------------------------------------------------------
Name (time in us)                                                                        Min                    Max                   Mean                StdDev                 Median                   IQR            Outliers       OPS            Rounds  Iterations
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path3-libpy_simdjson-loads]                                     869.1500 (1.0)       5,930.8430 (2.49)      1,123.0447 (1.0)        203.5337 (1.52)      1,112.6140 (1.0)         81.2050 (1.02)        29;35  890.4365 (1.0)         722           1
test_benchmark_load[path3-pysimdjson-parse]                                         875.8530 (1.01)      2,384.6440 (1.0)       1,127.0993 (1.00)       133.4820 (1.0)       1,119.4185 (1.01)        79.8040 (1.0)         72;38  887.2333 (1.00)        786           1
test_benchmark_load[path3-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]      6,227.8560 (7.17)     23,751.5410 (9.96)     10,109.4607 (9.00)     5,149.2095 (38.58)     7,506.5770 (6.75)     1,209.3185 (15.15)       27;29   98.9172 (0.11)        120           1
test_benchmark_load[path3-orjson-loads]                                           6,445.2870 (7.42)     23,814.3440 (9.99)     10,414.0837 (9.27)     4,872.0612 (36.50)     7,897.2230 (7.10)     2,016.7280 (25.27)       26;26   96.0238 (0.11)        114           1
test_benchmark_load[path3-pysimdjson_as_py_obj-loads]                             7,940.2930 (9.14)     24,984.5340 (10.48)    11,900.6726 (10.60)    5,518.1670 (41.34)     8,878.9950 (7.98)     3,686.7190 (46.20)       21;21   84.0289 (0.09)         90           1
test_benchmark_load[path3-ujson-loads]                                            8,323.1930 (9.58)     23,228.4760 (9.74)     12,104.0124 (10.78)    5,363.4790 (40.18)     9,344.6530 (8.40)       802.3722 (10.05)       19;20   82.6172 (0.09)         83           1
test_benchmark_load[path3-python_json-loads]                                     12,697.9430 (14.61)    31,279.0080 (13.12)    16,691.7376 (14.86)    5,163.8324 (38.69)    14,104.4160 (12.68)    2,390.4543 (29.95)       17;17   59.9099 (0.07)         75           1
test_benchmark_load[path3-rapidjson-loads]                                       14,025.9210 (16.14)    29,509.2470 (12.37)    17,785.5409 (15.84)    5,061.8217 (37.92)    15,456.7050 (13.89)    1,756.3833 (22.01)         7;7   56.2254 (0.06)         35           1
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



--------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/github_events.json': 8 tests ---------------------------------------------------------------
Name (time in us)                                                                     Min                   Max                Mean              StdDev              Median                IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path2-libpy_simdjson-loads]                                   25.4010 (1.0)      1,960.0230 (1.28)      36.2258 (1.0)       24.5956 (1.0)       34.6020 (1.0)       2.9000 (1.04)      72;1589       27.6046 (1.0)       12642           1
test_benchmark_load[path2-pysimdjson-parse]                                       25.8010 (1.02)     1,528.8010 (1.0)       37.9300 (1.05)      26.3332 (1.07)      37.3020 (1.08)      2.8010 (1.0)      189;1856       26.3644 (0.96)      16103           1
test_benchmark_load[path2-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     142.1090 (5.59)     3,436.0130 (2.25)     207.5540 (5.73)      88.5276 (3.60)     204.8130 (5.92)     17.6010 (6.28)       58;289        4.8180 (0.17)       4034           1
test_benchmark_load[path2-orjson-loads]                                          175.7120 (6.92)     5,736.6740 (3.75)     254.9315 (7.04)     162.0033 (6.59)     239.3160 (6.92)     20.8005 (7.43)       26;226        3.9226 (0.14)       2872           1
test_benchmark_load[path2-pysimdjson_as_py_obj-loads]                            224.7150 (8.85)     2,283.7510 (1.49)     321.5699 (8.88)      72.4103 (2.94)     324.0220 (9.36)     26.0020 (9.28)       79;180        3.1097 (0.11)       2632           1
test_benchmark_load[path2-ujson-loads]                                           301.7190 (11.88)    7,409.5770 (4.85)     375.2140 (10.36)    180.4753 (7.34)     363.8240 (10.51)    29.0010 (10.35)      18;175        2.6651 (0.10)       2269           1
test_benchmark_load[path2-python_json-loads]                                     330.5200 (13.01)    2,521.5590 (1.65)     459.9910 (12.70)     86.1825 (3.50)     455.8290 (13.17)    38.7277 (13.83)     119;146        2.1740 (0.08)       1909           1
test_benchmark_load[path2-rapidjson-loads]                                       380.6250 (14.98)    2,082.2340 (1.36)     533.6130 (14.73)     91.0874 (3.70)     529.9340 (15.32)    43.7030 (15.60)       76;95        1.8740 (0.07)       1709           1
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




----------------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/mesh.json': 8 tests ------------------------------------------------------------------------
Name (time in us)                                                                       Min                    Max                   Mean                StdDev                 Median                 IQR            Outliers       OPS            Rounds  Iterations
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path4-libpy_simdjson-loads]                                    994.5520 (1.0)       4,240.8210 (1.06)      1,202.4742 (1.0)        194.6273 (1.04)      1,190.2620 (1.0)       93.9050 (1.31)        80;42  831.6187 (1.0)         772           1
test_benchmark_load[path4-pysimdjson-parse]                                      1,027.3570 (1.03)      4,001.3210 (1.0)       1,257.5495 (1.05)       187.9555 (1.0)       1,253.6190 (1.05)      71.9545 (1.0)         37;50  795.1973 (0.96)        804           1
test_benchmark_load[path4-pysimdjson_as_py_obj-loads]                            2,782.4560 (2.80)     16,753.7350 (4.19)      3,907.4828 (3.25)     2,316.1314 (12.32)     3,456.5430 (2.90)     264.9645 (3.68)         9;18  255.9192 (0.31)        252           1
test_benchmark_load[path4-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     2,936.7520 (2.95)     19,099.6830 (4.77)      4,066.0482 (3.38)     2,579.4705 (13.72)     3,459.0780 (2.91)     291.3400 (4.05)        10;21  245.9390 (0.30)        207           1
test_benchmark_load[path4-orjson-loads]                                          3,848.3120 (3.87)     16,809.8280 (4.20)      4,853.2711 (4.04)     2,387.4255 (12.70)     4,348.8370 (3.65)     282.5400 (3.93)         8;13  206.0466 (0.25)        203           1
test_benchmark_load[path4-ujson-loads]                                           4,224.0310 (4.25)     27,094.9610 (6.77)      5,393.1933 (4.49)     2,651.7927 (14.11)     4,906.3150 (4.12)     368.6210 (5.12)         7;13  185.4189 (0.22)        196           1
test_benchmark_load[path4-python_json-loads]                                     7,946.7190 (7.99)     21,914.9540 (5.48)      9,216.2188 (7.66)     2,458.6174 (13.08)     8,729.1600 (7.33)     425.9220 (5.92)          4;8  108.5044 (0.13)        108           1
test_benchmark_load[path4-rapidjson-loads]                                       8,958.0800 (9.01)     26,303.9020 (6.57)     10,523.4513 (8.75)     2,727.4495 (14.51)    10,081.1370 (8.47)     748.3645 (10.40)         4;4   95.0259 (0.11)        101           1
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


---------------------------------------------------------------------- benchmark 'Load /home/runner/work/libpy_simdjson/libpy_simdjson/libpy_simdjson/tests/jsonexamples/twitter.json': 8 tests ----------------------------------------------------------------------
Name (time in us)                                                                       Min                    Max                  Mean                StdDev                Median                 IQR            Outliers         OPS            Rounds  Iterations
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_load[path1-libpy_simdjson-loads]                                    329.3220 (1.0)       3,357.5250 (2.67)       440.8892 (1.0)         84.7542 (1.65)       441.1300 (1.0)       37.4280 (1.22)        64;75  2,268.1435 (1.0)        1637           1
test_benchmark_load[path1-pysimdjson-parse]                                        348.7250 (1.06)      1,258.3910 (1.0)        451.6477 (1.02)        51.2426 (1.0)        450.8825 (1.02)      30.8030 (1.0)       374;155  2,214.1149 (0.98)       2006           1
test_benchmark_load[path1-libpy_simdjson_as_py_obj-libpy_simdjson_as_py_obj]     2,206.2470 (6.70)     14,217.4510 (11.30)    2,759.9331 (6.26)     1,386.3121 (27.05)    2,551.0705 (5.78)     312.4220 (10.14)         2;3    362.3276 (0.16)         74           1
test_benchmark_load[path1-orjson-loads]                                          2,639.8850 (8.02)     15,075.4730 (11.98)    3,420.6474 (7.76)     1,657.3556 (32.34)    3,183.0750 (7.22)     215.1150 (6.98)         6;15    292.3423 (0.13)        270           1
test_benchmark_load[path1-ujson-loads]                                           3,304.1320 (10.03)    18,557.3880 (14.75)    4,286.1597 (9.72)     1,868.0725 (36.46)    4,021.4820 (9.12)     283.4200 (9.20)         5;14    233.3091 (0.10)        239           1
test_benchmark_load[path1-pysimdjson_as_py_obj-loads]                            3,319.1400 (10.08)    16,355.6780 (13.00)    4,237.6133 (9.61)     1,642.9272 (32.06)    3,982.8870 (9.03)     306.1198 (9.94)         6;16    235.9819 (0.10)        259           1
test_benchmark_load[path1-python_json-loads]                                     4,154.6810 (12.62)    17,041.0680 (13.54)    5,474.9570 (12.42)    1,520.8511 (29.68)    5,262.9090 (11.93)    484.4330 (15.73)         6;8    182.6498 (0.08)        190           1
test_benchmark_load[path1-rapidjson-loads]                                       5,184.5590 (15.74)    19,217.2170 (15.27)    6,598.9679 (14.97)    1,949.8404 (38.05)    6,315.0330 (14.32)    682.2923 (22.15)         5;7    151.5388 (0.07)        149           1
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------- benchmark 'Random attribute access': 2 tests --------------------------------------------------------------------------------------------
Name (time in ns)                                  Min                       Max                  Mean                StdDev                Median                 IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_at[python_json-loads]          800.0000 (1.0)      1,560,881.0000 (6.79)     1,190.4413 (1.0)      6,373.7290 (3.79)     1,100.0000 (1.0)      100.0000 (1.0)      122;3966      840.0246 (1.0)       88496           1
test_benchmark_at[libpy_simdjson-loads]     1,300.0000 (1.63)       229,812.0000 (1.0)      1,848.6849 (1.55)     1,683.4805 (1.0)      1,800.0000 (1.64)     400.0000 (4.00)      143;234      540.9251 (0.64)      57801           1
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


---------------------------------------------------------------------------------------------------- benchmark 'Random list access': 2 tests ----------------------------------------------------------------------------------------------------
Name (time in ns)                                         Min                       Max                   Mean                StdDev                 Median                   IQR            Outliers  OPS (Kops/s)            Rounds  Iterations
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_benchmark_list_access[python_json-loads]        700.0000 (1.0)        258,213.0000 (1.0)       1,061.7637 (1.0)      1,165.4286 (1.0)       1,000.0000 (1.0)        200.0000 (1.0)      570;5297      941.8292 (1.0)      149232           1
test_benchmark_list_access[libpy_simdjson-loads]     700.0000 (1.00)     1,340,168.0000 (5.19)     10,658.8974 (10.04)    9,211.5111 (7.90)     10,301.0000 (10.30)    9,300.0000 (46.50)    4637;493       93.8183 (0.10)      81961           1
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Legend:
  Outliers: 1 Standard Deviation from Mean; 1.5 IQR (InterQuartile Range) from 1st Quartile and 3rd Quartile.
  OPS: Operations Per Second, computed as 1 / Mean
```
