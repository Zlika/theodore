#!/usr/bin/python3
#
# This file is part of theodore, a Thomson emulator
# (https://github.com/Zlika/theodore).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# This script downloads the list of games for Thomson computers
# from the ScreenScraper site and generates a DAT file for RetroArch.
# Requirements: lxml

from collections import namedtuple, OrderedDict
import datetime
import re
import urllib.request


SCREENSCRAPER_URL = "https://www.screenscraper.fr/"
SCREENSCRAPER_ROMSINFOS_URL = SCREENSCRAPER_URL + "romsinfos.php"
SCREENSCRAPER_ROMINFOS_URL = SCREENSCRAPER_URL + "rominfos.php"
THOMSON_PLATFORMID = 141

Rom = namedtuple('Rom', ['filename', 'gamename', 'year', 'crc', 'md5', 'sha1'])

total_roms = 0


# Returns the list of ROM indexes for the given platform.
def get_screenscraper_rom_letters(platformId):
	base_url = SCREENSCRAPER_ROMSINFOS_URL + "?plateforme=" + str(platformId)
	with urllib.request.urlopen(base_url) as response:
		page = response.read().decode('utf-8')
	matches = re.finditer(r"romsinfos\.php\?plateforme=141&alpha=(.+)&numpage=0", page)
	return sorted(set([m.group(1) for m in matches]))


# Returns the number of pages for the ROMs of the given index.
def get_screenscraper_nb_pages(platformId, index):
	base_url = (SCREENSCRAPER_ROMSINFOS_URL + "?plateforme=" + str(platformId)
				+ "&alpha=" + index + "&numpage=0")
	with urllib.request.urlopen(base_url) as response:
		page = response.read().decode('utf-8')
	matches = re.finditer(r"romsinfos\.php\?plateforme=141&alpha=" + index + "&numpage=([0-9]+)", page)
	return len(set([m.group(1) for m in matches]))


# Returns the name of the file.
def read_rom_filename(page):
	return re.search(r'<div class="cssnavigationcolonne">(.+?)<\/div><\/td>', page).group(1)


# Returns the name of the game.
def read_rom_gamename(page, platformId):
	return re.search(r'<a href="gameinfos\.php\?plateforme=' + str(platformId)
					+ '&gameid=[0-9]+">(.+?)<\/a><br>', page).group(1)


# Returns the CRC of the ROM (or None if not available).
def read_rom_crc(page):
	match = re.search(r'>CRC :<\/td>.*?<td colspan=3 nowrap class="csstabletdtext">([0-9a-fA-F]*?)<\/td>', page)
	return None if not match else match.group(1).strip()


# Returns the MD5 of the ROM (or None if not available).
def read_rom_md5(page):
	match = re.search(r'>MD5 :<\/td>.*?<td colspan=3 nowrap class="csstabletdtext">([0-9a-fA-F]*?)<\/td>', page)
	return None if not match else match.group(1).strip()


# Returns the SHA1 of the ROM (or None if not available).
def read_rom_sha1(page):
	match = re.search(r'>SHA1 :<\/td>.*?<td colspan=3 nowrap class="csstabletdtext">([0-9a-fA-F]*?)<\/td>', page)
	return None if not match else match.group(1).strip()


# Returns the year of the game (or None if not available).
def read_rom_year(page):
	match = re.search(r'<td class="cssadmintabletdwhite" style="text-align: left">([0-9]{4})<\/td>', page)
	return None if not match else match.group(1).strip()


# Returns the info data for the given ROM id.
def get_screenscraper_rom_by_id(platformId, romid):
	url = SCREENSCRAPER_ROMINFOS_URL + "?romid=" + str(romid)
	with urllib.request.urlopen(url) as response:
		page = response.read().decode('utf-8')
	page_single_line = page.replace('\n', '').replace('\r', '')
	filename = read_rom_filename(page_single_line)
	gamename = read_rom_gamename(page_single_line, platformId)
	crc = read_rom_crc(page_single_line)
	md5 = read_rom_md5(page_single_line)
	sha1 = read_rom_sha1(page_single_line)
	year = read_rom_year(page_single_line)
	return Rom(filename=filename, gamename=gamename, year=year, crc=crc, md5=md5, sha1=sha1)


# Print a ROM data in the dat format
def print_dat_rom(rom):
	global total_roms
	total_roms += 1
	print('')
	print('game (')
	print('\tname "' + rom.gamename + '"')
	print('\tdescription "' + rom.gamename + '"')
	if rom.year:
		print('\tyear "' + rom.year + '"')
	romline = '\trom ( name "' + rom.filename + '"'
	if rom.crc:
		romline += ' crc ' + rom.crc.strip().lower()
	if rom.md5:
		romline += ' md5 ' + rom.md5.strip().lower()
	if rom.sha1:
		romline += ' sha1 ' + rom.sha1.strip().lower()
	romline += ' )'
	print(romline)
	print(')', flush=True)


# Returns the list of ROMs for the given index.
def print_screenscraper_roms_for_index(platformId, index):
	nb_pages = get_screenscraper_nb_pages(platformId, index)
	for i in range(nb_pages):
		url = (SCREENSCRAPER_ROMSINFOS_URL + "?plateforme=" + str(platformId)
				+ "&alpha=" + index + "&numpage=" + str(i))
		with urllib.request.urlopen(url) as response:
			page = response.read().decode('utf-8')
		matches = re.finditer(r"rominfos\.php\?romid=([0-9]+)", page)
		for id in list(OrderedDict.fromkeys([m.group(1) for m in matches])):
			print_dat_rom(get_screenscraper_rom_by_id(platformId, id))


# Print the header part of a dat file
def print_dat_header():
	print('clrmamepro (')
	print('\tname "Thomson - MO/TO"')
	now = datetime.datetime.now()
	print('\tversion "' + now.strftime('%Y%m%d-%H%M%S') + '"')
	print('\thomepage "https://github.com/Zlika/theodore/libretro-database"')
	print('\tcomment "https://www.screenscraper.fr"')
	print(')', flush=True)


if __name__ == "__main__":
	print_dat_header()
	indexes = get_screenscraper_rom_letters(THOMSON_PLATFORMID)
	for index in indexes:
		print_screenscraper_roms_for_index(THOMSON_PLATFORMID, index)
	print(total_roms + ' roms')
