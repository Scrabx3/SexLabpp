import argparse

argparser = argparse.ArgumentParser(description="Read a hash file and print its contents.")
argparser.add_argument("-b", "--binary", type=str, default=0, help="Binary hash key")
argparser.add_argument("-d", "--decimal", type=str, default=0, help="Binary hash key")
args = argparser.parse_args()
if args.binary == 0 and args.decimal == 0:
  print("Please provide a hash key using -b or -d")
  exit(1)

# Mapping from \src\Registry\Define\Fragment.h
FRAGMENT_FLAGS = {
    "Male":        1 << 0,
    "Female":      1 << 1,
    "Human":       1 << 2,
    "Vampire":     1 << 3,
    "Futa":        1 << 4,
    "CrtBit0":     1 << 3,
    "CrtBit1":     1 << 4,
    "CrtBit2":     1 << 5,
    "CrtBit3":     1 << 6,
    "CrtBit4":     1 << 7,
    "CrtBit5":     1 << 8,
    "Submissive":  1 << 9,
    "Unconscious": 1 << 10,
}
crt_bits = FRAGMENT_FLAGS["CrtBit0"] | FRAGMENT_FLAGS["CrtBit1"] | FRAGMENT_FLAGS["CrtBit2"] | FRAGMENT_FLAGS["CrtBit3"] | FRAGMENT_FLAGS["CrtBit4"] | FRAGMENT_FLAGS["CrtBit5"]

def print_flags_from_binary(value):
  is_human = value & FRAGMENT_FLAGS["Human"]
  for name, bit in FRAGMENT_FLAGS.items():
    if not value & bit:
      continue
    if name.startswith("CrtBit"):
      continue
    if not is_human and crt_bits & bit:
      continue
    print(f"{name} ({bit})")
  if not is_human:
    crt_key = (value & crt_bits) >> 3
    print(f"Creature: {crt_key} ({crt_key:b})")

def read_hash_file(hash_key):
  if len(hash_key) == 55:
    part_length = len(hash_key) // 5
    parts = [hash_key[i*part_length:(i+1)*part_length] for i in range(5)]
  else:
    parts = [hash_key]
  for part in parts:
    value = int(part, 2)
    if value == 0:
      continue
    print(f"Value: {part}")
    print_flags_from_binary(value)
    print("")

binary_arg = args.binary
if args.decimal != 0:
  binary_arg = bin(int(args.decimal))[2:]

read_hash_file(binary_arg)
