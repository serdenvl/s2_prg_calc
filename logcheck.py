def do_things(logstr: str):
    ads = tuple('0x' + s.replace('\n', ' ').split(' ')[0] for s in logstr.split('0x')[1:])
    return (ad for ad in set(ads) if ads.count(ad) % 2 != 0)

if __name__ == '__main__':
    while True:
        print('Ввод:')
        print('Путь к файлу -> проверка файла')
        print('Пустая строка -> ./mem_check_log.txt')
        print('end -> завершение')
        inp = input()
        if inp == 'end':
            break
        if not inp:
            inp = 'mem_check_log.txt'
            
        try:
            with open(inp, 'r') as file:
                bads = '\n'.join(do_things(file.read()))
                print('Неосвобожденные адреса: \n' + bads if bads else 'Всё ок')
        except FileNotFoundError:
            print('not found')
        except Exception as err:
            print(err)
        
        print()
