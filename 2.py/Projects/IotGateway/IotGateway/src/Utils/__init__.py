
import importlib,sys,os
importlib.reload(sys)
sys.setdefaultencoding('utf8')

from Utils import PlatformIdentify
if PlatformIdentify.isWindows():
    activate_this = os.path.join(os.path.dirname(os.path.abspath(__file__)),os.path.pardir,os.path.pardir,'webapp/venv/Scripts/activate_this.py')
    execfile(activate_this, dict(__file__=activate_this))
else:
    #linux
    activate_this = os.path.join(os.path.dirname(os.path.abspath(__file__)),os.path.pardir,os.path.pardir,'webapp/venv/bin/activate_this.py')
    execfile(activate_this, dict(__file__=activate_this))
