
// 12306Client_MFCView.cpp : CMy12306Client_MFCView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "12306Client_MFC.h"
#endif

#include "12306Client_MFCDoc.h"
#include "12306Client_MFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy12306Client_MFCView

IMPLEMENT_DYNCREATE(CMy12306Client_MFCView, CView)

BEGIN_MESSAGE_MAP(CMy12306Client_MFCView, CView)
END_MESSAGE_MAP()

// CMy12306Client_MFCView 构造/析构

CMy12306Client_MFCView::CMy12306Client_MFCView()
{
	// TODO: 在此处添加构造代码

}

CMy12306Client_MFCView::~CMy12306Client_MFCView()
{
}

BOOL CMy12306Client_MFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy12306Client_MFCView 绘制

void CMy12306Client_MFCView::OnDraw(CDC* /*pDC*/)
{
	CMy12306Client_MFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CMy12306Client_MFCView 诊断

#ifdef _DEBUG
void CMy12306Client_MFCView::AssertValid() const
{
	CView::AssertValid();
}

void CMy12306Client_MFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy12306Client_MFCDoc* CMy12306Client_MFCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy12306Client_MFCDoc)));
	return (CMy12306Client_MFCDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy12306Client_MFCView 消息处理程序


