# -*- coding: utf-8 -*-
"""
Created on Tue Nov 12 01:35:11 2019

@author: wolai
"""

import traceback

def stack():

    print('The python stack:')

    traceback.print_stack()

from twisted.internet import reactor

reactor.callWhenRunning(stack)

reactor.run()