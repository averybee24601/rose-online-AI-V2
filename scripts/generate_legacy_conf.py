#!/usr/bin/env python3
import argparse
import os
import sys
import yaml

LEGACY_KEYS = {
    'login': {
        'file': 'loginserver.conf',
        'map': {
            'mysql.host': 'mysql_host',
            'mysql.port': 'mysql_port',
            'mysql.database': 'mysql_database',
            'mysql.user': 'mysql_user',
            'mysql.password': 'mysql_pass',
            'server.id': 'serverid',
            'server.type': 'servertype',
            'server.name': 'servername',
            'server.ip': 'serverip',
            'server.port': 'serverport',
            'server.lan_ip': 'lanip',
            'server.lan_mask': 'lansubmask',
            'server.login_pass': 'loginpass',
            'server.char_pass': 'charpass',
            'server.world_pass': 'worldpass',
            'server.min_access_level': 'accesslevel',
            'server.use_threads': 'usethreads',
            'server.create_login_account': 'CreateLoginAccount',
            'server.check_gameguard': 'checkGameGuard',
            # optional
            'server.parent_id': 'parentid',
        }
    },
    'char': {
        'file': 'charserver.conf',
        'map': {
            'mysql.host': 'mysql_host',
            'mysql.port': 'mysql_port',
            'mysql.database': 'mysql_database',
            'mysql.user': 'mysql_user',
            'mysql.password': 'mysql_pass',
            'server.id': 'serverid',
            'server.type': 'servertype',
            'server.name': 'servername',
            'server.ip': 'serverip',
            'server.port': 'serverport',
            'server.lan_ip': 'lanip',
            'server.lan_mask': 'lansubmask',
            'server.login_pass': 'loginpass',
            'server.char_pass': 'charpass',
            'server.world_pass': 'worldpass',
            'server.use_threads': 'usethreads',
            'server.parent_id': 'parentid',
            # additional
            'server.delete_time': 'deletetime',
            'server.min_access_level': 'accesslevel',
        }
    },
    'world': {
        'file': 'worldserver.conf',
        'map': {
            'mysql.host': 'mysql_host',
            'mysql.port': 'mysql_port',
            'mysql.database': 'mysql_database',
            'mysql.user': 'mysql_user',
            'mysql.password': 'mysql_pass',
            'server.id': 'serverid',
            'server.type': 'servertype',
            'server.name': 'servername',
            'server.ip': 'serverip',
            'server.port': 'serverport',
            'server.lan_ip': 'lanip',
            'server.lan_mask': 'lansubmask',
            'server.login_pass': 'loginpass',
            'server.char_pass': 'charpass',
            'server.world_pass': 'worldpass',
            'server.use_threads': 'usethreads',
            'server.parent_id': 'parentid',
            # world specifics
            'rates.exp': 'EXP_RATE',
            'rates.drop': 'DROP_RATE',
            'rates.zuly': 'ZULY_RATE',
            'rates.item_drop': 'ItemDropRate',
            'gameplay.autosave': 'AUTOSAVE',
            'gameplay.autosave_seconds': 'SAVETIME',
            'server.min_access_level': 'minimal_access_level',
        }
    }
}

def get_nested(cfg, path):
    cur = cfg
    for part in path.split('.'):
        if isinstance(cur, dict) and part in cur:
            cur = cur[part]
        else:
            return None
    return cur


def emit_legacy(cfg, mapping):
    lines = []
    for path, key in mapping.items():
        val = get_nested(cfg, path)
        if val is None:
            continue
        if isinstance(val, bool):
            val = 1 if val else 0
        lines.append(f"{key} = {val}")
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser(description='Generate legacy .conf files from YAML configs')
    ap.add_argument('--config-dir', default='config', help='Directory containing YAML configs')
    ap.add_argument('--out-dir', default='.', help='Output directory for legacy .conf files')
    args = ap.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)

    for srv, spec in LEGACY_KEYS.items():
        yml_name = f"{srv}server.yml" if srv != 'login' else 'loginserver.yml'
        yml_path = os.path.join(args.config_dir, yml_name)
        if not os.path.isfile(yml_path):
            print(f"skip: {yml_path} not found", file=sys.stderr)
            continue
        with open(yml_path, 'r', encoding='utf-8') as f:
            cfg = yaml.safe_load(f) or {}
        content = emit_legacy(cfg, spec['map'])
        out_path = os.path.join(args.out_dir, spec['file'])
        with open(out_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"wrote {out_path}")

if __name__ == '__main__':
    main()
