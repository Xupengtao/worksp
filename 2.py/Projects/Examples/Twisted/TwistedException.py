# -*- coding: utf-8 -*-
"""
Created on Tue Nov 12 01:57:12 2019

@author: wolai
"""

def falldown():
    raise Exception('I fall down.')

def upagain():
    print('But I get up again.')
    reactor.stop()

from twisted.internet import reactor
reactor.callWhenRunning(falldown)
reactor.callWhenRunning(upagain)

print('Starting the reactor.')
reactor.run()