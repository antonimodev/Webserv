from os import environ


def hello():
    print("Content-Type: text/html")
    print()
    for num in range(1, 11):
        print(num)
    print(f"\nSCRIPT_FILENAME (ruta del archivo) = {environ.get('SCRIPT_FILENAME')}")
    print(f"\nPATH_INFO (lo que hay despues del script) = {environ.get('PATH_INFO')}")
    print(f"query: {environ.get('QUERY_STRING')}")


if __name__ == "__main__":
    hello()
