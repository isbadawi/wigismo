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

def get_field(name, type):
    'Returns the form input with the given name, or a suitable default.'
    try:
        return type(fields.getvalue(name, ''))
    except ValueError:
        return type()

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

    def set(self, name, value):
        try:
            f = open(self.service, 'r')
            d = pickle.Unpickler(f).load()
            d[name] = value
            f.close()
            f = open(self.service, 'w')
            pickle.Pickler(f).dump(d)
            return value
        finally:
            f.close()

    def set_key(self, name, key, value):
       try:
            f = open(self.service, 'r')
            d = pickle.Unpickler(f).load()
            d[name][key] = value
            f.close()
            f = open(self.service, 'w')
            pickle.Pickler(f).dump(d)
            return value
       finally:
            f.close()  

def tuple_keep(t, *ids):
    result = {}
    for key in t:
        if key in ids:
            result[key] = t[key]
    return result

def tuple_discard(t, *ids):
    result = {}
    for key in t:
        if key not in ids:
            result[key] = t[key]
    return result

def tuple_combine(t1, t2):
    result = t1.copy()
    result.update(t2)
    return result

def set(d, key, val):
    d[key] = val
    return val

def set_key(d, name, key, val):
    d[name][key] = val
    return val
