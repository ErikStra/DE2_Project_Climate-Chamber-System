import os
import time

def before_build(env, platform):
    timestamp = str(time.time())
    with open("src/build_time.h", "w") as f:
        f.write(f"#define BUILD_TIMESTAMP \"{timestamp}\"\n")
