import ast
import glob
import os
import sys

from libpy.build import LibpyExtension
from setuptools import find_packages, setup

if ast.literal_eval(os.environ.get("LIBPY_SIMDJSON_DEBUG_BUILD", "0")):
    optlevel = 0
    debug_symbols = True
    max_errors = 5
else:
    optlevel = 3
    debug_symbols = False
    max_errors = None


def extension(*args, **kwargs):
    extra_compile_args = ["-DLIBPY_AUTOCLASS_UNSAFE_API"]
    if sys.platform == "darwin":
        extra_compile_args.append("-mmacosx-version-min=10.15")

    return LibpyExtension(
        *args,
        optlevel=optlevel,
        debug_symbols=debug_symbols,
        werror=True,
        max_errors=max_errors,
        include_dirs=(
            [".", "submodules/range-v3/include/"] + kwargs.pop("include_dirs", [])
        ),
        extra_compile_args=extra_compile_args,
        depends=glob.glob("**/*.h", recursive=True),
        **kwargs
    )


install_requires = [
    "setuptools",
    "libpy",
]

setup(
    name="libpy_simdjson",
    version="0.3.0",
    description="Python bindings for smidjson, using libpy",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/gerrymanoim/libpy_simdjson",
    author="Gerry Manoim, Joe Jevnik",
    author_email=(
        "Gerry Manoim <gerrymanoim@gmail.com>, Joe Jevnik <joejev@gmail.com>"
    ),
    packages=find_packages(),
    license="Apache 2.0",
    classifiers=[
        "Development Status :: 4 - Beta",
        "License :: OSI Approved :: Apache Software License",
        "Natural Language :: English",
        "Topic :: Software Development",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: Implementation :: CPython",
        "Programming Language :: C++",
        "Operating System :: POSIX",
        "Intended Audience :: Developers",
    ],
    # we need the headers to be available to the C compiler as regular files;
    # we cannot be imported from a ziparchive.
    zip_safe=False,
    install_requires=install_requires,
    extras_require={
        "test": ["pytest"],
        "benchmark": [
            "pytest-benchmark",
            "orjson",
            "python-rapidjson",
            "pysimdjson",
            "ujson",
        ],
    },
    ext_modules=[
        extension(
            "libpy_simdjson.parser",
            ["libpy_simdjson/parser.cc", "libpy_simdjson/simdjson.cpp"],
        ),
    ],
)
