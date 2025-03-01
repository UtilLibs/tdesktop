/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "ui/widgets/input_fields.h"
#include "base/timer.h"
#include "base/qt_connection.h"
#include "chat_helpers/compose/compose_features.h"

#ifndef TDESKTOP_DISABLE_SPELLCHECK
#include "boxes/dictionaries_manager.h"
#include "spellcheck/spelling_highlighter.h"
#endif // TDESKTOP_DISABLE_SPELLCHECK

#include <QtGui/QClipboard>

namespace Main {
class Session;
class SessionShow;
} // namespace Main

namespace Window {
class SessionController;
} // namespace Window

namespace ChatHelpers {
enum class PauseReason;
class Show;
} // namespace ChatHelpers

namespace Ui {
class PopupMenu;
} // namespace Ui

QString PrepareMentionTag(not_null<UserData*> user);
TextWithTags PrepareEditText(not_null<HistoryItem*> item);

Fn<bool(
	Ui::InputField::EditLinkSelection selection,
	QString text,
	QString link,
	Ui::InputField::EditLinkAction action)> DefaultEditLinkCallback(
		std::shared_ptr<Main::SessionShow> show,
		not_null<Ui::InputField*> field,
		const style::InputField *fieldStyle = nullptr);
void InitMessageFieldHandlers(
	not_null<Main::Session*> session,
	std::shared_ptr<Main::SessionShow> show, // may be null
	not_null<Ui::InputField*> field,
	Fn<bool()> customEmojiPaused,
	Fn<bool(not_null<DocumentData*>)> allowPremiumEmoji = nullptr,
	const style::InputField *fieldStyle = nullptr);
void InitMessageFieldHandlers(
	not_null<Window::SessionController*> controller,
	not_null<Ui::InputField*> field,
	ChatHelpers::PauseReason pauseReasonLevel,
	Fn<bool(not_null<DocumentData*>)> allowPremiumEmoji = nullptr);
void InitMessageField(
	std::shared_ptr<ChatHelpers::Show> show,
	not_null<Ui::InputField*> field,
	Fn<bool(not_null<DocumentData*>)> allowPremiumEmoji);
void InitMessageField(
	not_null<Window::SessionController*> controller,
	not_null<Ui::InputField*> field,
	Fn<bool(not_null<DocumentData*>)> allowPremiumEmoji);

void InitSpellchecker(
	std::shared_ptr<Main::SessionShow> show,
	not_null<Ui::InputField*> field,
	bool skipDictionariesManager = false);

bool HasSendText(not_null<const Ui::InputField*> field);

struct InlineBotQuery {
	QString query;
	QString username;
	UserData *bot = nullptr;
	bool lookingUpBot = false;
};
InlineBotQuery ParseInlineBotQuery(
	not_null<Main::Session*> session,
	not_null<const Ui::InputField*> field);

struct AutocompleteQuery {
	QString query;
	bool fromStart = false;
};
AutocompleteQuery ParseMentionHashtagBotCommandQuery(
	not_null<const Ui::InputField*> field,
	ChatHelpers::ComposeFeatures features);

class MessageLinksParser : private QObject {
public:
	MessageLinksParser(not_null<Ui::InputField*> field);

	void parseNow();

	[[nodiscard]] const rpl::variable<QStringList> &list() const;

protected:
	bool eventFilter(QObject *object, QEvent *event) override;

private:
	struct LinkRange {
		int start;
		int length;
		QString custom;
	};
	friend inline bool operator==(const LinkRange &a, const LinkRange &b) {
		return (a.start == b.start)
			&& (a.length == b.length)
			&& (a.custom == b.custom);
	}
	friend inline bool operator!=(const LinkRange &a, const LinkRange &b) {
		return !(a == b);
	}

	void parse();
	void apply(const QString &text, const QVector<LinkRange> &ranges);

	not_null<Ui::InputField*> _field;
	rpl::variable<QStringList> _list;
	int _lastLength = 0;
	base::Timer _timer;
	base::qt_connection _connection;

};

[[nodiscard]] base::unique_qptr<Ui::RpWidget> CreateDisabledFieldView(
	QWidget *parent,
	not_null<PeerData*> peer);
