import time

def before_build(env, platform):
    with open("src/dummy.h", "w") as f:
        f.write(f"#define DUMMY_TIMESTAMP \"{time.ctime()}\"\n")

