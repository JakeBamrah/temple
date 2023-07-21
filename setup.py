# Setup temple as a local package so that it can be imported in child files,
# e.g. unittests. Install package locally: "pip install -e ." from root dir
from setuptools import setup, find_packages

setup(name='temple', version='1.0', packages=find_packages())
