# !python3
# !/usr/bin/python3
# -*- coding: utf-8 -*-

# .SYNOPSIS
# .DESCRIPTION
# .EXAMPLE
# .INPUTS
#      None
# .OUTPUTS
#      None
# .NOTES
#      Author: Anatoly Evdokimov
#      Date: January 06 2018

import re
import config
import requests
import telebot
import threading
from datetime import datetime
from emoji import emojize
from telebot import types

bot = telebot.TeleBot(config.token, threaded=False)

wink = emojize(":wink:", use_aliases=True)
smiling_imp = emojize(":smiling_imp:", use_aliases=True)
expressionless = emojize(":expressionless:", use_aliases=True)


@bot.message_handler(commands=['help', 'start'])
def send_welcome(message):
    msg = bot.send_message(message.chat.id, 'Введи ключ для доступа к управлению')
    bot.register_next_step_handler(msg, check_key)


def check_key(message):
    key = message.text
    if key in config.keys:
        bot.send_message(message.chat.id, 'Введен правильный ключ')
        set_keyboard(message)
    else:
        bot.send_message(message.chat.id, 'Неа, ты не из нашего кабинета ' + smiling_imp)
        send_welcome(message)
        return


def set_keyboard(message):
    ckeyboard = types.ReplyKeyboardMarkup(resize_keyboard=True)
    ckeyboard.add('Показать температуру и влажность')
    ckeyboard.add('Включить', 'Выключить')
    ckeyboard.add('Режим \"АВТО\"')
    ckeyboard.add('Режим \"Холод\"', 'Режим \"Комфорт\"')
    bot.send_message(message.chat.id,
                      'Привет! Я бот управляющий кондеем. Чем могу помочь, дружище?',
                      reply_markup=ckeyboard)


@bot.message_handler(content_types=['text'])
def main(message):
    if message.text == 'Показать температуру и влажность':
        get_status(message)
    elif message.text == 'Включить':
        requests.post(config.server + '/$1')
        bot.send_message(message.chat.id, 'Включен режим 21C + Auto')
    elif message.text == 'Выключить':
        requests.post(config.server + '/$2')
    elif message.text == 'Режим \"АВТО\"':
        requests.post(config.server + '/$3')
        bot.send_message(message.chat.id, 'Включен режим 22C + Dry + Swing')
    elif message.text == 'Режим \"Холод\"':
        requests.post(config.server + '/$4')
        bot.send_message(message.chat.id, 'Включен режим 19C + Cool + Fan High')
    elif message.text == 'Режим \"Комфорт\"':
        requests.post(config.server + '/$5')
        bot.send_message(message.chat.id, 'Включен режим 22C + Dry')


def get_status(message):
    response = requests.get(config.server)
    temp = re.search(r'\d+.\d\sC', response.text)
    hum = re.search(r'\d+.\d\s%', response.text)
    bot.send_message(message.chat.id, 'Сейчас в кабинете: температура - ' + temp.group(0) +
                     ', влажность - ' + hum.group(0))


bot.polling(none_stop=True, timeout=60)
