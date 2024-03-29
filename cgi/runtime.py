#!/usr/bin/python
import cPickle as pickle
import cgi
import os
import string
import random

fields = cgi.FieldStorage()
sessionid = os.environ.get('QUERY_STRING', 'default')

def random_string(length):
    'Generates a random string of length lowercase characters.'
    return ''.join([random.choice(string.lowercase) for i in range(length)])

def get_field(name):
    'Returns the form input with the given name, or the the empty string.'
    return fields.getvalue(name, '')

def output(session, out, exit=False):
    """
    Outputs a Content-type declaration and wraps the given out function 
    (which outputs an HTML literal) with an HTML form if necessary.
    """
    print 'Content-type: text/html'
    print
    if not exit: 
        print '<form method="POST" action="?%s">' % session
    out()
    if not exit: 
        print '<p><input type="submit" value="go">'
        print '</form>'

class Store(object):
    'A simplistic key-value store which reads/writes items from/to a file.'
    def __init__(self, service):
        self.service = service
        if not os.path.exists(self.service):
            f = open(self.service, 'w')
            pickle.Pickler(f).dump({})
            f.close()

    def get_all(self):
        try:
            f = open(self.service, 'r')
            return pickle.Unpickler(f).load()
        finally:
            f.close()

    def get(self, name, default=None):
        try:
            f = open(self.service, 'r')
            return pickle.Unpickler(f).load().get(name, default)
        finally:
            f.close()

    def put(self, name, value):
        try:
            f = open(self.service, 'r')
            d = pickle.Unpickler(f).load()
            d[name] = value
            f.close()
            f = open(self.service, 'w')
            pickle.Pickler(f).dump(d)
        finally:
            f.close()

