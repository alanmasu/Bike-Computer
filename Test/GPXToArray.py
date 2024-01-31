import xml.etree.ElementTree as ET
import os

def leggi_file_gpx(filename):
    albero = ET.parse(filename)
    radice = albero.getroot()

    punti = []

    for trkpt in radice.findall(".//{http://www.topografix.com/GPX/1/1}trkpt"):
        lat = trkpt.get("lat")
        lon = trkpt.get("lon")
        ele = trkpt.findtext("{http://www.topografix.com/GPX/1/1}ele", default="0")
        time = trkpt.findtext("{http://www.topografix.com/GPX/1/1}time", default="")
        punti.append({"latitude": str(lat), "longitude": str(lon), "altitude": str(ele), "time": time})

    return punti

def genera_file_h(filename, array_name="CoordinateArray"):
    coordinate = leggi_file_gpx(filename)

    with open("Test/GPX_Points.h", "w") as f:
        f.write("#ifndef GPX_POINTS_H\n")
        f.write("#define GPX_POINTS_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("typedef struct {\n")
        f.write("\tchar latitude[20]; // Modifica la dimensione se necessario\n")
        f.write("\tchar longitude[20]; // Modifica la dimensione se necessario\n")
        f.write("\tchar altitude[20]; // Modifica la dimensione se necessario\n")
        f.write("\tchar time[30]; // Modifica la dimensione se necessario\n")
        f.write("} GPSCoordinate;\n\n")
        f.write(f"static const GPSCoordinate {array_name}[] = {{\n")

        for punto in coordinate:
            f.write(f"\t{{ \"{punto['latitude']}\", \"{punto['longitude']}\", \"{punto['altitude']}\", \"{punto['time']}\" }},\n")

        f.write("};\n\n")
        f.write(f"static const uint32_t {array_name}_size = {len(coordinate)};\n\n")
        f.write("#endif // GPX_POINTS_H\n")

def main():
    filename = "Test/gpxTest.gpx"
    genera_file_h(filename)

if __name__ == "__main__":
    main()
