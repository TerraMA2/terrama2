import json, sys

dataEn = json.loads(open('../webapp/locales/en_US.json').read())
dataPt = json.loads(open('../webapp/locales/pt_BR.json').read())
dataEs = json.loads(open('../webapp/locales/es_ES.json').read())
dataFr = json.loads(open('../webapp/locales/fr_FR.json').read())

ptError = False
esError = False
frError = False

for k, v in dataEn.items():
    if(k not in dataPt):
        ptError = True

    if(k not in dataEs):
        esError = True

    #if(k not in dataFr):
        #frError = True

    if(ptError and esError and frError):
        break

if(ptError or esError or frError):
    exitMessage = "Error in translation of the following languages: "

if(ptError):
    exitMessage += "Portuguese, "

if(esError):
    exitMessage += "Spanish, "

if(frError):
    exitMessage += "French, "

if(ptError or esError or frError):
    sys.stdout.write(exitMessage[:-2] + "\n\n")
    sys.exit(-1)
else:
    sys.exit(0)