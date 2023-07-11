from sys import argv
import os, datetime, math

if len(argv) != 4:

    print('error| writefile.py exactly takes 3 arguments.')
    
    quit()

(_, dest, mbr, src_dir) = argv

with open(dest, 'wb+') as dest_file:

    dest_file.truncate(1440 * 1024) # 1440kb

    with open(mbr, 'rb') as mbr_file:

        mbr_file.seek(0, 2)

        if mbr_file.tell() != 512:

            print('error| mbr must be exactly 512 bytes.')
            print(mbr_file.tell())
            quit()

        mbr_file.seek(0)
        dest_file.seek(0)

        dest_file.write(mbr_file.read())

    clusters = [0xff0, 0xfff]
    nextClusterId = 2

    def visitDir(dir: str, writeTo: int):
        global nextClusterId
        fileId = 0

        fileNames = os.listdir(dir)

        if 'cosplay.sys' in fileNames: # Make cosplay.sys become the first file

            fileNames.remove('cosplay.sys')
            fileNames.insert(0, 'cosplay.sys')

        for name in fileNames:

            fullName = os.path.join(dir, name)

            if os.path.isdir(fullName):

                continue

            (base, ext) = os.path.splitext(name)

            ext = ext.strip('.')

            converted = (base[:8].upper(), ext[:3].upper())

            if len(base) >= 8 and len(ext) >= 3:

                print(f'"{name}" is not a DOS-like file name (8.3 format).')

            dest_file.seek(writeTo + fileId * 32)
            dest_file.write(converted[0].encode())
            dest_file.seek(writeTo + fileId * 32 + 8)
            dest_file.write(converted[1].encode())
            dest_file.seek(writeTo + fileId * 32 + 11)
            dest_file.write(b'\x20')
            dest_file.seek(10, 1)

            createdTime = datetime.datetime.fromtimestamp(os.path.getctime(fullName))

            dest_file.write(((abs(createdTime.year - 1980) << 9) | (createdTime.month << 5) | createdTime.day).to_bytes(2, 'little'))
            dest_file.write(((createdTime.hour << 11) | (createdTime.minute << 5)).to_bytes(2, 'little'))
            dest_file.write(nextClusterId.to_bytes(2, 'little'))
            dest_file.write(os.path.getsize(fullName).to_bytes(4, 'little'))

            clustersUsed = math.ceil(os.path.getsize(fullName) / 256)

            for i in range(clustersUsed):

                clusters.append(nextClusterId + i + 1)

            clusters[-1] = 0xfff

            dest_file.seek(nextClusterId * 512 + 0x3e00)

            with open(fullName, 'rb') as src_file:

                dest_file.write(src_file.read())

            nextClusterId += clustersUsed

            fileId += 1

    visitDir(src_dir, 0x2600)

    if len(clusters) % 2 == 1:
        
        clusters.append(0)

    def writeFat(offset: int):

        dest_file.seek(offset)
        
        for i in range(0, len(clusters), 2):

            dest_file.write((clusters[i] | (clusters[i + 1] << 12)).to_bytes(3, 'little'))

    writeFat(0x200)
    writeFat(0x1400)