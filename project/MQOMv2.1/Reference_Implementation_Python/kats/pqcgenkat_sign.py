import sys, os
sys.path.insert(1, "mqom/")
sys.path.insert(1, "kats/")
sys.path.insert(1, ".")
from mqom import MQOM2
import re
from rng import randombytes_init, randombytes
from joblib import Parallel, delayed
import time

# Number of KATS
NUM_KATS = 100
VERBOSE = True


def crypto_sign(scheme, sk, msg):
    sig = scheme.sign(sk, msg)
    return msg + sig

def crypto_sign_open(scheme, pk, sm):
    sig_bytesize = scheme.get_signature_bytesize()
    if len(sm) < sig_bytesize:
        raise Exception()
    msg, sig = sm[:-sig_bytesize], sm[-sig_bytesize:]
    scheme.verify(pk, msg, sig)
    return msg

def execute_one_kat_sig(seed_dict_entry):
    seed = seed_dict_entry['seed']
    label = seed_dict_entry['label']
    count = seed_dict_entry['count']
    mlen = seed_dict_entry['mlen']
    msg = seed_dict_entry['msg']
    scheme = seed_dict_entry['scheme']
    # Seed the random generator
    randombytes_init(seed, None, 256)
    # Generate the keys
    start = time.time()
    try:
        (pk, sk) = scheme.generate_keys()
    except Exception as e:
        raise Exception('crypto_sign_keypair failed') from e
    timing = time.time() - start
    if VERBOSE is True:
        print(f'[{label}] KeyGen Timing: {timing}')

    assert len(pk) == scheme.get_public_key_bytesize()
    assert len(sk) == scheme.get_secret_key_bytesize()
    # Sign
    start = time.time()
    sm = crypto_sign(scheme, sk, msg)
    timing = time.time() - start
    if VERBOSE is True:
        print(f'[{label}] Sign Timing: {timing}')
    smlen = len(sm)
    #start = time.time()
    #msg2 = crypto_sign_open(scheme, pk, sm)
    #timing = time.time() - start
    #if VERBOSE is True:
    #    print(f'[{label}] Open Sign Timing: {timing}')
    #assert msg == msg2
    # Return the value
    return {'seed': seed, 'label': label, 'count': count, 'mlen': mlen, 'msg': msg, 'pk': pk, 'sk': sk, 'smlen': smlen, 'sm': sm, 'scheme': scheme }

def generate_kat(label, params, parallel=1):
    global scheme
    scheme = MQOM2(params, randombytes)

    # Create a folder for the dedicated instance
    out_folder_name = 'build/'+label[6:].replace("-", "_").replace('L', 'cat')+'/'
    try:
        os.makedirs(out_folder_name)
    except:
        print("Error when creating folder '%s' (already exists?) ..." % out_folder_name)
        sys.exit(-1)

    sk_bytesize = scheme.get_secret_key_bytesize()
    fn_req = out_folder_name + f'PQCsignKAT_{sk_bytesize}.req'
    fn_rsp = out_folder_name + f'PQCsignKAT_{sk_bytesize}.rsp'

    entropy_input = bytes([
        i for i in range(48)
    ])

    randombytes_init(entropy_input, None, 256)

    # Store the seeds in place in a dict
    seed_dict = { }
    with open(fn_req, 'w') as fp_req:
        for i in range(NUM_KATS):
            fp_req.write(f'count = {i}\n')
            seed = randombytes(48)
            fp_req.write(f'seed = {seed.hex().upper()}\n')
            mlen = 33*(i+1)
            fp_req.write(f'mlen = {mlen}\n')
            msg = randombytes(mlen)
            fp_req.write(f'msg = {msg.hex().upper()}\n')
            fp_req.write('pk =\n')
            fp_req.write('sk =\n')
            fp_req.write('smlen =\n')
            fp_req.write('sm =\n\n')

    # Save the order of the seed list
    seed_list = []
    count_kat = 0 
    with open(fn_req, 'r') as fp_req:
        regex_count = re.compile(r'count = (\d+)')
        regex_seed = re.compile(r'seed = ([0-9A-F]{96})')
        regex_mlen = re.compile(r'mlen = (\d+)')
        regex_msg = re.compile(r'msg = ([0-9A-F]+)')

        done = False
        while not done:
            full_line = fp_req.readline()
            line = full_line.strip()
            while (line is not None) and ((res := regex_count.fullmatch(line)) is None) and (full_line != ""):
                full_line = fp_req.readline()
                line = full_line.strip()
            if (line is None) or (full_line == ""):
                done = True
                break
            count_kat += 1
            count = int(res.group(1))

            res = regex_seed.fullmatch(fp_req.readline().strip())
            assert res
            seed = bytes.fromhex(res.group(1))
            randombytes_init(seed, None, 256)

            res = regex_mlen.fullmatch(fp_req.readline().strip())
            assert res
            mlen = int(res.group(1))

            res = regex_msg.fullmatch(fp_req.readline().strip())
            assert res
            msg = bytes.fromhex(res.group(1))
            assert len(msg) == mlen
            seed_list.append({'seed': seed, 'label': label, 'count': count, 'mlen': mlen, 'msg': msg, 'scheme': scheme})

    assert count_kat == NUM_KATS

    ############ Parallelized executions
    results = Parallel(n_jobs=parallel, backend="loky")(map(delayed(execute_one_kat_sig), seed_list)) 

    # Sort the results following "count"
    results = sorted(results, key=lambda d: d['count'])

    ############ Save the results in order
    with open(fn_rsp, 'w') as fp_rsp:
        fp_rsp.write(f'# {label}\n\n')
        for s in results:
            seed = s['seed']
            count = s['count']
            mlen = s['mlen']
            msg = s['msg']
            pk = s['pk']
            sk = s['sk']
            smlen = s['smlen']
            sm = s['sm']
            #
            fp_rsp.write(f'count = {count}\n')
            fp_rsp.write(f'seed = {seed.hex().upper()}\n')
            fp_rsp.write(f'mlen = {mlen}\n')
            fp_rsp.write(f'msg = {msg.hex().upper()}\n')
            fp_rsp.write(f'pk = {pk.hex().upper()}\n')
            fp_rsp.write(f'sk = {sk.hex().upper()}\n')
            fp_rsp.write(f'smlen = {len(sm)}\n')
            fp_rsp.write(f'sm = {sm.hex().upper()}\n\n')
