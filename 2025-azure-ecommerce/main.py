import uuid
import json
import os

import pandas as pd
import streamlit as st

from azure.storage.blob import BlobServiceClient
from dotenv import load_dotenv
from sqlalchemy import create_engine, text

load_dotenv()

# Configurações do Azure Blob Storage
STORAGE_ACCOUNT_NAME = os.getenv("STORAGE_ACCOUNT_NAME")
STORAGE_CONNECTION_STRING = os.getenv("STORAGE_CONNECTION_STRING")
CONTAINER_NAME = os.getenv("CONTAINER_NAME")

# Configurações do Azure SQL Server via SQLAlchemy + pyodbc
SQL_SERVER = os.getenv("SQL_SERVER")
SQL_DATABASE = os.getenv("SQL_DATABASE")
SQL_USERNAME = os.getenv("SQL_USERNAME")
SQL_PASSWORD = os.getenv("SQL_PASSWORD")

# String de conexão compatível com SQLAlchemy + pyodbc
conn_str = (
    f"mssql+pyodbc://{SQL_USERNAME}:{SQL_PASSWORD}@{SQL_SERVER}:1433/{SQL_DATABASE}"
    "?driver=ODBC+Driver+17+for+SQL+Server"
)
engine = create_engine(conn_str)

# Application title
st.title("Cadastro de Produtos - e-Commerce na Nuvem")

# Formulário para cadastro de produtos
product_name = st.text_input("Nome do Produto")
product_description = st.text_area("Descrição do Produto")
product_price = st.number_input("Preço do Produto", min_value=0.0, format="%.2f")
product_image = st.file_uploader("Imagem do Produto", type=["jpg", "jpeg", "png"])

# Função para enviar imagem para o Azure Blob Storage
def upload_image(file):
    try:
        blob_service_client = BlobServiceClient.from_connection_string(STORAGE_CONNECTION_STRING)
        container_client = blob_service_client.get_container_client(CONTAINER_NAME)

        # Cria um nome único para a imagem
        blob_name = f"{uuid.uuid4()}.jpg"
        blob_client = container_client.get_blob_client(blob_name)

        # Faz o upload da imagem
        blob_client.upload_blob(file.read(), overwrite=True)

        # Monta a URL da imagem
        image_url = f"https://{STORAGE_ACCOUNT_NAME}.blob.core.windows.net/{CONTAINER_NAME}/{blob_name}"
        return image_url
    except Exception as e:
        st.error(f"Erro ao enviar imagem: {e}")
        return None


# Função para inserir os dados do produto no Azure SQL Server usando SQLAlchemy
def insert_product_sql(product_data):
    try:
        with engine.begin() as connection:
            connection.execute(text("""
                INSERT INTO dbo.Produtos (nome, descricao, preco, imagem_url)
                VALUES (:nome, :descricao, :preco, :imagem_url)
            """), product_data)
        return True
    except Exception as e:
        st.error(f"Erro ao inserir produto no banco de dados: {e}")
        return False


# Função para listar produtos usando SQLAlchemy
def list_products_sql():
    try:
        with engine.connect() as connection:
            result = connection.execute(text("""
                        SELECT id, nome, descricao, preco, imagem_url FROM dbo.Produtos
                    """))
            rows = [dict(row._mapping) for row in result.fetchall()]
        return rows
    except Exception as e:
        st.error(f"Erro ao listar produtos: {e}")
        return []


# Função para exibir os produtos
def list_produtos_screen():
    products = list_products_sql()
    if products:
        cards_por_linha = 3
        cols = st.columns(cards_por_linha)
        for i, product in enumerate(products):
            col = cols[i % cards_por_linha]
            with col:
                st.markdown(f"### {product['nome']}")
                st.write(f"**Descrição:** {product['descricao']}")
                st.write(f"**Preço:** R$ {product['preco']:.2f}")
                if product["imagem_url"]:
                    html_img = f'<img src="{product["imagem_url"]}" width="200" height="200" alt="Imagem do produto">'
                    st.markdown(html_img, unsafe_allow_html=True)
                st.markdown("---")
            if (i + 1) % cards_por_linha == 0 and (i + 1) < len(products):
                cols = st.columns(cards_por_linha)
    else:
        st.info("Nenhum produto encontrado.")


# Botão para cadastro do produto
if st.button("Cadastrar Produto"):
    if not product_name or not product_description or product_price is None:
        st.warning("Preencha todos os campos obrigatórios!")
    else:
        image_url = ""
        if product_image is not None:
            image_url = upload_image(product_image)

        product_data = {
            "nome": product_name,
            "descricao": product_description,
            "preco": product_price,
            "imagem_url": image_url
        }

        if insert_product_sql(product_data):
            st.success("Produto cadastrado com sucesso no Azure SQL!")
            list_produtos_screen()
        else:
            st.error("Houve um problema ao cadastrar o produto no Azure SQL.")

        # Salvando os dados localmente em JSON
        file_path = "produtos.json"
        if os.path.exists(file_path):
            with open(file_path, "r", encoding="utf-8") as f:
                try:
                    produtos = json.load(f)
                except json.JSONDecodeError:
                    produtos = []
        else:
            produtos = []

        produtos.append(product_data)
        with open(file_path, "w", encoding="utf-8") as f:
            json.dump(produtos, f, ensure_ascii=False, indent=4)

        st.json(product_data)


st.header("Listagem dos Produtos")

if st.button("Listar Produtos"):
    list_produtos_screen()
