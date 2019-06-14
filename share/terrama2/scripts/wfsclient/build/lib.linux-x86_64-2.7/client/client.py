from owslib.wfs import WebFeatureService
from owslib.feature.schema import get_schema
import os

class Client:

    def __init__(self, address, version=None, username=None, password=None):
      self.wfs = WebFeatureService(url=address, version=version, username=username, password=password)

    def get_title(self):
      return self.wfs.identification.title

    def get_feature(self, tpname, box, srs, outputFormat=None):
      return self.wfs.getfeature(typename=tpname, bbox=box, srsname=srs, outputFormat=outputFormat)

    def get_operations_avaliable(self):
      return [operation.name for operation in self.wfs.operations]

    def get_attribute_list(self, tpname):
      return get_schema(self.wfs.url, typename=tpname, version=self.wfs.version, username=self.wfs.username, password=self.wfs.password)

    def get_features_types_list(self):
      return list(self.wfs.contents)

    def convert_to_shp(self, path, featurename, response):
      if os.path.isdir(path):
        out = open(os.path.join(path, featurename+'.zip'), 'wb')
        out.write(response.read())
        out.close()
        return os.path.isfile(os.path.join(path, featurename+'.zip'))
      else:
        return False