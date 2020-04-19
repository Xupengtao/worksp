class SBProtocol():
    
    connection_count=0
    countPendingCmd=0
    
    def __init__(self):
        self.m_buffer               = ""
        self.dictWaitResp           = {}
        self.dictControlling        = {}
        self.HeaderTagType          = -1 #-1: not decided, 0: no header_tag, 1: has header_tag
        self.rcv_alarm              = "False"
        self.role                   = ""
    def DirSelf(self):
        print(dir(self))

if __name__ == '__main__':
    Sbp = SBProtocol()
    Sbp.DirSelf()