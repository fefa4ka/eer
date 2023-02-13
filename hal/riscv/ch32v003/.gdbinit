target extended-remote :3333
# Reset the target and call its init script
monitor reset init
# Halt the target. The init script should halt the target, but just in case
monitor halt

b main

monitor resume
monitor halt
monitor reset init
cont
