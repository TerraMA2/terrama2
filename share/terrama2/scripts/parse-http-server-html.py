#!/usr/bin/python

# @author Jean Souza [jean.souza@funcate.org.br]
from bs4 import BeautifulSoup

soup = BeautifulSoup("{HTML_CODE}", "html.parser")
files = ""

for el in soup.findAll('tr'):
  td = el.findAll('td')

  if td is not None and len(td) > 1 and td[1].find('a').text != 'Parent Directory':
    files += td[1].find('a').text + ","

if len(files) > 0:
  files = files[:-1]
