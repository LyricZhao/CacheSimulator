from matplotlib import pyplot as plt

def draw(data, gather, ax, ay, name):
    plt.figure()
    reduce_map = {}
    for d in data:
        key = '_'.join([d[field] for field in gather]).lower()
        if key not in reduce_map:
            reduce_map[key] = []
        reduce_map[key].append(d)
    for key in reduce_map:
        data = reduce_map[key]
        x = [float(d[ax]) for d in data]
        y = [float(d[ay]) for d in data]
        plt.plot(x, y, label=key, linewidth=1, alpha=0.6)
    plt.legend()
    plt.savefig('figures/{}.png'.format(name))

def check(entry, fields):
    for i in range(len(fields)):
        if fields[i] != '' and entry[i] != fields[i]:
            return False
    return True

def data_filter(req, data):
    fields = []
    for name in names:
        fields.append(req[name] if name in req else '')
    result = []
    exists = set()
    for entry in data:
        if entry in exists:
            continue
        exists.add(entry)
        entry = entry.split(',')
        if not check(entry, fields):
            continue
        new_dict = {}
        for i in range(len(names)):
            new_dict[names[i]] = entry[i]
        result.append(new_dict)
    return result

def exp1(raw, case):
    # exp1-meta
    data = data_filter({'case': case, 'replace': 'LRU', 'write_allocate': 'WRITE_ALLOCATE', 'write_policy': 'WRITE_BACK'}, raw)
    draw(data, ['case', 'type'], 'block_size', 'meta', 'exp1-meta')

    # exp1-case
    data = data_filter({'case': case, 'replace': 'LRU', 'write_allocate': 'WRITE_ALLOCATE', 'write_policy': 'WRITE_BACK'}, raw)
    draw(data, ['case', 'type'], 'block_size', 'rate', 'exp1-{}'.format(case))

def exp2(raw, case):
    pass

def exp3(raw, case):
    pass

if __name__ == "__main__":
    with open('output.csv', 'r') as csv:
        lines = csv.readlines()
        names = lines[0].split(',')
        data = lines[1:]
    
    for case in ['astar', 'bzip2', 'mcf', 'perlbench']:
        exp1(data, case)
        exp2(data, case)
        exp3(data, case)