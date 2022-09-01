from typing import List


def dict_update(left: dict, right: dict) -> dict:
    result = dict()
    for left_key, left_value in left.items():
        if left_key not in right:
            result[left_key] = left_value
        else:
            if type(left_value) != type(right[left_key]):
                result[left_key] = right[left_key]
            else:
                if isinstance(left_value, dict):
                    result[left_key] = dict_update(left_value, right[left_key])
                else:
                    result[left_key] = right[left_key]
    for right_key, right_value in right.items():
        if right_key not in result:
            result[right_key] = right_value
    return result

def dict_from_list(d: List[dict], key_name: str) -> dict:
    result = dict()
    for entry in d:
        key_list = entry.pop(key_name)
        for k in key_list:
            result[k] = entry
    return result