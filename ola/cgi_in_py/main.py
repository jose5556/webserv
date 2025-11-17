#!/usr/bin/env python3


import os
import cgi
import cgitb
import html
import sys

cgitb.enable()

UPLOAD_DIR = "/tmp"  # diretório onde os arquivos serão salvos


# ---------------- Cabeçalho HTTP ----------------
def cabecalho_http():
    print("Content-Type: text/html; charset=UTF-8")
    print("Cache-Control: no-store, no-cache, must-revalidate")
    print("Pragma: no-cache")
    print()


# ---------------- Página inicial ----------------
def pagina_principal_body():
    return """\
<html>
  <head>
    <meta charset="utf-8">
    <title>Pagina com Botoes</title>
  </head>
  <body>
    <h1>Escolha uma das opcoes:</h1>
    <form method="get" action="/cgi-bin/pagina_principal.py">
      <input type="submit" name="pagina" value="Pagina 1">
      <input type="submit" name="pagina" value="Pagina 2">
      <input type="submit" name="pagina" value="Pagina 3">
      <input type="submit" name="pagina" value="Pagina 4">
      <input type="submit" name="pagina" value="Pagina 5">
    </form>
    <p>Escolha uma opção para continuar.</p>
  </body>
</html>
"""


# ---------------- Páginas dinâmicas ----------------
def pagina_dinamica(pagina, form=None):
    paginas = {
        "Pagina 1": "Voce esta na Pagina 1",
        "Pagina 2": "Voce esta na Pagina 2",
        "Pagina 3": "Envio de ficheiro",
        "Pagina 4": "Variaveis de ambiente",
        "Pagina 5": "Voce esta na Pagina 5"
    }

    if pagina == "Pagina 3":
        resultado_upload = ""
        if form is not None:
            ficheiro_item = form["ficheiro"] if "ficheiro" in form else None
            if ficheiro_item and ficheiro_item.filename:
                filename = os.path.basename(ficheiro_item.filename)
                save_path = os.path.join(UPLOAD_DIR, filename)
                try:
                    with open(save_path, "wb") as f:
                        f.write(ficheiro_item.file.read())
                    resultado_upload = f"<p>Ficheiro '{html.escape(filename)}' enviado com sucesso para {html.escape(UPLOAD_DIR)}</p>"
                except Exception as e:
                    resultado_upload = f"<p>Erro ao guardar ficheiro: {html.escape(str(e))}</p>"
            else:
                erro_file_path = os.path.join(UPLOAD_DIR, "erro_arquivo_nao_enviado.txt")
                try:
                    with open(erro_file_path, "w") as f:
                        f.write("Erro: Nenhum ficheiro foi enviado.\n")
                    resultado_upload = f"<p>Nenhum ficheiro enviado. Foi criado o arquivo vazio: '{html.escape(erro_file_path)}'</p>"
                except Exception as e:
                    resultado_upload = f"<p>Erro ao criar arquivo vazio: {html.escape(str(e))}</p>"

        return f"""
        <h1>Enviar um ficheiro</h1>
        {resultado_upload}
        <form method="post" action="/cgi-bin/pagina_principal.py" enctype="multipart/form-data">
            <input type="hidden" name="pagina" value="Pagina 3">
            <input type="file" name="ficheiro">
            <input type="submit" value="Enviar">
        </form>
        <p><a href="/cgi-bin/pagina_principal.py">Voltar para a pagina inicial</a></p>
        """

    elif pagina == "Pagina 4":
        env_items = sorted(os.environ.items())
        rows = []
        for k, v in env_items:
            rows.append(
                "<tr>"
                f"<td style='vertical-align:top;padding:4px;border:1px solid #ccc'><strong>{html.escape(k)}</strong></td>"
                f"<td style='vertical-align:top;padding:4px;border:1px solid #ccc'>{html.escape(v)}</td>"
                "</tr>"
            )
        tabela = (
            "<h1>Variaveis de ambiente recebidas pelo CGI</h1>"
            f"<p>Total: {len(env_items)}</p>"
            "<table style='border-collapse:collapse'>"
            "<thead><tr><th style='padding:6px;border:1px solid #ccc'>Nome</th><th style='padding:6px;border:1px solid #ccc'>Valor</th></tr></thead>"
            "<tbody>"
            + "".join(rows) +
            "</tbody></table>"
            "<p><a href=\"/cgi-bin/pagina_principal.py\">Voltar para a pagina inicial</a></p>"
        )
        return tabela

    else:
        conteudo = paginas.get(pagina, "Pagina nao encontrada!")
        return f"<h1>{html.escape(conteudo)}</h1><p>Conteudo da {html.escape(pagina.lower())}.</p>"


# ---------------- Função principal ----------------
def main():
    os.environ.setdefault("PATH_INFO", "/")
    os.environ.setdefault("QUERY_STRING", "")
    os.environ.setdefault("REQUEST_METHOD", "GET")
    os.environ.setdefault("SCRIPT_NAME", sys.argv[0])

    # Detectar se temos CONTENT_LENGTH (modo CGI clássico)
    has_content_length = "CONTENT_LENGTH" in os.environ and os.environ["CONTENT_LENGTH"].isdigit()

    form = None
    pagina = None

    if has_content_length:
        try:
            form = cgi.FieldStorage()
            pagina = form.getvalue("pagina") if form else None
        except Exception as e:
            print("Status: 400 Bad Request")
            cabecalho_http()
            print(f"<h1>Erro ao processar dados do formulario:</h1><pre>{html.escape(str(e))}</pre>")
            return
    else:
        # Se não há CONTENT_LENGTH, lemos o stdin diretamente (modo chunked / stdin cru)
        pagina = None

    # --- Ler o corpo da requisição cru (sempre até EOF) ---
    try:
        body_raw = sys.stdin.buffer.read()
        body_content = body_raw.decode("utf-8", errors="replace")
    except Exception as e:
        body_content = f"(Erro ao ler corpo da mensagem: {e})"

    # Cabeçalhos HTTP
    cabecalho_http()

    # Conteúdo principal
    if pagina:
        print(pagina_dinamica(pagina, form))
    else:
        print(pagina_principal_body())

    # Mostrar corpo recebido
    print("<hr>")
    print("<h3>Corpo recebido na requisição:</h3>")
    if body_content.strip():
        print(f"<pre style='background:#f8f8f8;padding:8px;border:1px solid #ccc'>{html.escape(body_content)}</pre>")
    else:
        print("<p><em>Nenhum conteúdo foi enviado no corpo da requisição.</em></p>")


if __name__ == "__main__":
    main()

