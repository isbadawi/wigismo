import cPickle as pickle
import cgi
import os

fields = cgi.FieldStorage()

class Globals(object):
    def __init__(self, service):
        self.service = service
        if (os.path.exists(self.service)):
            try:
                with open(self.service, 'r') as f:
                    pickle.Unpickler(f).load()
                    return
            except EOFError:
                pass
        with open(self.service, 'w') as f:
            pickle.Pickler(f).dump({})


    def get_global(self, name):
        with open(self.service, 'r') as f:
            return pickle.Unpickler(f).load().get(name, '')

    def put_global(self, name, value):
        with open(self.service, 'r') as f:
            d = pickle.Unpickler(f).load()
            d[name] = value
        with open(self.service, 'w') as f:
            pickle.Pickler(f).dump(d)

def get_field(name):
    return fields.getvalue(name, '')
