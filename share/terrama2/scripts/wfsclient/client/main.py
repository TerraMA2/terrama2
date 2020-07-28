from client import Client

def getwfslayer(args):

  host = args["host"]
  version = args["version"]
  feature = args["feature"]
  xmin = args["xmin"]
  ymin = args["ymin"]
  xmax = args["xmax"]
  ymax = args["ymax"]
  projection = args["projection"]
  temp_folder = args["tempFolder"]

  #initialize client
  client_wfs = Client(host, version)
  client_wfs.get_attribute_list(feature)

  #Get feature
  response = client_wfs.get_feature(feature,
                                   (xmin, ymin, xmax, ymax),
                                    projection, 'shape-zip')

  #Generating shape in disk
  print('Is it was generated?')
  print(client_wfs.convert_to_shp(temp_folder[7:], feature, response))

  return temp_folder