import ctypes, sys

if sys.platform.startswith("linux") :

  import os, glob, site
  from distutils.core import setup

  setup(
      name = "WFSClient",
      version = "1.0.0",
      author = "TerraMA2 Team",
      author_email = "terrama2@dpi.inpe.br",
      description = ("A python client for WFS"),
      py_modules = ['client'],
      license = "LGPL",
      keywords = "terrama2 geo",
      url = "http://www.terrama2.dpi.inpe.br/",
      packages = ["client"],
  )