import os
import sys
from shapeworks import *

success = True

def dicomSeriesUIDsTest():
  uids = Image.getDICOMSeriesUIDs(os.environ["DATA"] + "/dcm_files")
  return len(uids) > 0 and len(uids[0]) > 0

success &= utils.test(dicomSeriesUIDsTest)

sys.exit(not success)
