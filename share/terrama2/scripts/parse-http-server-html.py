#!/usr/bin/python

#
# This script aims to retrieve the names of datasets from HTML content
# TerraMA2 will replace the special tag with data provider page and then
# list all datasets associated. Once that, performs validation through node links
#
# **REMEMBER** that the data provider must offer dataset links using tag <a>
#
# @author Jean Souza [jean.souza@funcate.org.br]
from bs4 import BeautifulSoup

soup = BeautifulSoup('''{HTML_CODE}''', "html.parser")
files = ""

def getLink(aElement):
  '''Retrieves html direct link of <a> element'''
  return aElement.text

for aElement in soup.findAll('a'):
  link = getLink(aElement)

  if link != "../":
    files += (link if not link.endswith('/') else link[:len(link)-1]) + ','

if len(files) > 0:
  files = files[:-1]