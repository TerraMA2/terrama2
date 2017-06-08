import json, sys, unicodedata

dataEn = json.loads(open('../webapp/locales/en_US.json').read())
dataPt = json.loads(open('../webapp/locales/pt_BR.json').read())
dataEs = json.loads(open('../webapp/locales/es_ES.json').read())
dataFr = json.loads(open('../webapp/locales/fr_FR.json').read())

ptItens = []
esItens = []
frItens = []

for k, v in dataEn.items():
  if(k not in dataPt):
    ptItens.append(unicodedata.normalize("NFKD", k).encode('ascii', 'ignore'))

  if(k not in dataEs):
    esItens.append(unicodedata.normalize("NFKD", k).encode('ascii', 'ignore'))

  if(k not in dataFr):
    frItens.append(unicodedata.normalize("NFKD", k).encode('ascii', 'ignore'))

if(len(ptItens) == 0):
  sys.stdout.write("\n\n-------------------------------------------------\n\nThe Portuguese translation file is correct.\n\n")
else:
  sys.stdout.write("\n\n-------------------------------------------------\n\nMissing translations in Portuguese:\n\n")

  for s in ptItens:
    sys.stdout.write("  " + s + "\n")

if(len(esItens) == 0):
  sys.stdout.write("\n\n-------------------------------------------------\n\nThe Spanish translation file is correct.\n\n")
else:
  sys.stdout.write("\n\n-------------------------------------------------\n\nMissing translations in Spanish:\n\n")

  for s in esItens:
    sys.stdout.write("  " + s + "\n")

if(len(frItens) == 0):
  sys.stdout.write("\n\n-------------------------------------------------\n\nThe French translation file is correct\n\n.")
else:
  sys.stdout.write("\n\n-------------------------------------------------\n\nMissing translations in French:\n\n")

  for s in frItens:
    sys.stdout.write("  " + s + "\n")