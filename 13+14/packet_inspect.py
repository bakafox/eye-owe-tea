# Тестер отправки пакетов -- ДЛЯ РАБОТЫ НА В*НДЕ НУЖНО ПОСТАВИТЬ npcap !!!

# pip install scapy libpcap
from scapy.all import sniff, TCP, IP


def print_data(packet):
    if packet.haslayer(TCP) and packet.haslayer(IP):
        print(f'\nНОВЫЙ ПАКЕТ!!! УРА!!!')
        print(f'Откуда: {packet[IP].src}:{packet[TCP].sport}')
        print(f'Куда: {packet[IP].dst}:{packet[TCP].dport}')
        print(f'Флажки: {packet[TCP].flags}')
        
        if packet.payload:
            data = bytes(packet.payload)
            try:
                print(f'Данные: {data.decode("utf-8")}')
            except:
                print(f'Сырые данные: {data}')


i_face = input('На этом компе стоит б-гомерзская В*нда? [Y 🤢/N 🥰]: ')
i_face = '\\Device\\NPF_Loopback' if i_face.strip().capitalize() == 'Y' else 'lo'

port = input('Введите порт (например, 12345): ')
port = int(port) if port else 12345


print(f'Начинаю слушать пакеты на порту {port}!')

sniff(
    filter=f"tcp port {port}",
    iface=i_face,
    prn=print_data,
    store=False
)
