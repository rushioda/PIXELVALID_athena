# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def _local_apply_core(func, args, q):
    import os
    try:
        q.put(func(*args))
    except BaseException, e:
        q.put(e)
        os._exit(1)

def apply(func, args):
    import random
    from Queue import Empty
    from multiprocessing import Process
    from multiprocessing.managers import SyncManager
    import tempfile
    import os
    if os.getcwd()[:4] != '/afs':
        try:
            m = SyncManager(address=tempfile.mktemp(prefix='dqu_subprocess-',
                                                    dir=os.getcwd()))
            m.start()
        except EOFError:
            m = SyncManager()
            m.start()
    else:
        m = SyncManager()
        m.start()
    q = m.Queue()
    p = Process(target=_local_apply_core, args=(func, args, q))
    p.start()
    p.join()
    print 'Manager socket is', m.address
    try:
        rv = q.get(False)
    except Empty:
        raise RuntimeError('daughter died while trying to execute %s%s' % (func.func_name, args))
    if isinstance(rv, BaseException):
        if isinstance(rv, SystemExit):
            print 'SystemExit raised by daughter; ignoring'
            return None
        else:
            raise rv
    m.shutdown()
    return rv
