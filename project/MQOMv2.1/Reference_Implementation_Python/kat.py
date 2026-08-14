import argparse
from kats.pqcgenkat_sign import generate_kat
from mqom import Category, TradeOff, Variant, MQOM2Parameters
from labels import get_label, get_instance_from_label

all_variants = " "
for cat in [Category.I, Category.III, Category.V]:
    for field_size in [2, 16, 256]:
        for trade_off in [TradeOff.SHORT, TradeOff.FAST]:
            for variant in [Variant.R5, Variant.R3]:
                all_variants += get_label(cat, field_size, trade_off, variant) + ", "

parser = argparse.ArgumentParser()
parser.add_argument('-l', nargs='+', action='append', dest='label', help='List of schemes to test: one of {%s}' % all_variants)
parser.add_argument('-p', '--parallel-jobs', dest='parallel_jobs', type=int, default=1, help='Number of parallel jobs (-1 means max, 1 means monojob)')
arguments = parser.parse_args()

label = arguments.label
parallel_jobs = arguments.parallel_jobs

if label is None:
    all_labels = None
else:
    all_labels = [item for row in label for item in row]

if all_labels is None:
    print("[+] Testing all the variants for KAT")
    for cat in [Category.I, Category.III, Category.V]:
        for field_size in [2, 16, 256]:
            for trade_off in [TradeOff.SHORT, TradeOff.FAST]:
                for variant in [Variant.R5, Variant.R3]:
                    # Get MQOM2 instance
                    params = MQOM2Parameters.get(cat, field_size, trade_off, variant)
                    label = get_label(cat, field_size, trade_off, variant)
                    print("  [+] Testing %s" % label)
                    generate_kat(label, params, parallel=parallel_jobs)
else:
    print("[+] Testing %s for KAT" % all_labels)
    for label in all_labels:
        print("  [+] Testing %s" % label)
        params = get_instance_from_label(label)
        generate_kat(label, params, parallel=parallel_jobs)
